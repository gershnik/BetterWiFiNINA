/*
  This file is part of the BetterWiFiNINA library.
  Copyright (c) 2024 Eugene Gershnik. All rights reserved.
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or modify it under
  the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  This library is distributed in the hope that it will be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License 
  for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <https://www.gnu.org/licenses/>.
*/

#include "WiFiBearSSLSocket.h"

#include <errno.h>

#if BETTER_WIFI_NINA_HAS_BEAR_SSL

int WiFiBearSSLSocket::s_lastError = 0;

bool WiFiBearSSLSocket::handshake() {
    s_lastError = 0;
    while ( true ) {
        auto state = br_ssl_engine_current_state(m_engine);

        if (state == 0) {
            s_lastError = WiFiBearSSLSocket::NotInitialized;
            return false;
        }

        if (state == BR_SSL_CLOSED) {
            auto sslErr = br_ssl_engine_last_error(m_engine);
            if (sslErr != BR_ERR_OK) {
                s_lastError = WiFiBearSSLSocket::ProtocolError;
            } else {
                s_lastError = ECONNRESET;
            }
            return false;
        }
        
        if ((state & BR_SSL_SENDREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_sendrec_buf(m_engine, &bufLen);
            auto sent = m_socket.send(buf, bufLen);
            if (sent < 0) {
                s_lastError = WiFiSocket::lastError();
                return false;
            }
            br_ssl_engine_sendrec_ack(m_engine, sent);
            continue;
        }

        if ((state & BR_SSL_RECVREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_recvrec_buf(m_engine, &bufLen);
            auto read = m_socket.recv(buf, bufLen);
            if (read < 0) {
                s_lastError = WiFiSocket::lastError();
                return false;
            }
            br_ssl_engine_recvrec_ack(m_engine, read);
            continue;
        }

        if ((state & BR_SSL_RECVAPP) != 0 || (state & BR_SSL_SENDAPP) != 0) {
            return true;
        }

        //This should be unreachable... should we crash here or just return true unconditionally?
    }
}

int32_t WiFiBearSSLSocket::send(const void * src, uint16_t size) {

    s_lastError = 0;
    if (size == 0)
        return 0;

    uint16_t written = 0;
    while ( true ) {
        auto state = br_ssl_engine_current_state(m_engine);

        if (state == 0) {
            s_lastError = WiFiBearSSLSocket::NotInitialized;
            return -1;
        }

        if (state == BR_SSL_CLOSED) {
            auto sslErr = br_ssl_engine_last_error(m_engine);
            if (sslErr != BR_ERR_OK) {
                s_lastError = WiFiBearSSLSocket::ProtocolError;
            } else {
                s_lastError = ECONNRESET;
            }
            return -1;
        }

        if (written < size && (state & BR_SSL_SENDAPP) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_sendapp_buf(m_engine, &bufLen);
            do {
                if (bufLen > size_t(size) - written)
                    bufLen = size_t(size) - written;
                memcpy(buf, static_cast<const uint8_t *>(src) + written, bufLen);
                written += bufLen;
                br_ssl_engine_sendapp_ack(m_engine, bufLen);
                if (written == size)
                    break;
                buf = br_ssl_engine_sendapp_buf(m_engine, &bufLen);
            } while (buf);
            continue;
        }
        
        if ((state & BR_SSL_SENDREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_sendrec_buf(m_engine, &bufLen);
            auto sent = m_socket.send(buf, bufLen);
            if (sent < 0) {
                auto err = WiFiSocket::lastError();
                if (err == EWOULDBLOCK && written) {
                    return written;
                }
                s_lastError = err;
                return -1;
            }
            br_ssl_engine_sendrec_ack(m_engine, sent);
            continue;
        }

        if ((state & BR_SSL_RECVREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_recvrec_buf(m_engine, &bufLen);
            auto read = m_socket.recv(buf, bufLen);
            if (read < 0) {
                auto err = WiFiSocket::lastError();
                if (err == EWOULDBLOCK && written) {
                    return written;
                }
                s_lastError = err;
                return -1;
            }
            br_ssl_engine_recvrec_ack(m_engine, read);
            continue;
        }

        if (written)
            return written;

        s_lastError = WiFiBearSSLSocket::MustRecv;
        return -1;
    }
}

int32_t WiFiBearSSLSocket::recv(void * dest, uint16_t size) {
    s_lastError = 0;
    if (size == 0)
        return 0;

    uint16_t received = 0;
    while ( true ) {
        auto state = br_ssl_engine_current_state(m_engine);

        if (state == 0) {
            s_lastError = WiFiBearSSLSocket::NotInitialized;
            return -1;
        }

        if (state == BR_SSL_CLOSED) {
            auto sslErr = br_ssl_engine_last_error(m_engine);
            if (sslErr != BR_ERR_OK) {
                s_lastError = WiFiBearSSLSocket::ProtocolError;
            } else {
                s_lastError = ECONNRESET;
            }
            return -1;
        }

        if (received < size && (state & BR_SSL_RECVAPP) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_recvapp_buf(m_engine, &bufLen);
            do {
                if (bufLen > size_t(size) - received)
                    bufLen = size_t(size) - received;
                memcpy(static_cast<uint8_t *>(dest) + received, buf, bufLen);
                received += bufLen;
                br_ssl_engine_recvapp_ack(m_engine, bufLen);
                if (received == size)
                    break;
                buf = br_ssl_engine_recvapp_buf(m_engine, &bufLen);
            } while(buf);
            continue;
        }
        
        if ((state & BR_SSL_SENDREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_sendrec_buf(m_engine, &bufLen);
            auto sent = m_socket.send(buf, bufLen);
            if (sent < 0) {
                auto err = WiFiSocket::lastError();
                if (err == EWOULDBLOCK && received) {
                    return received;
                }
                s_lastError = err;
                return -1;
            }
            br_ssl_engine_sendrec_ack(m_engine, sent);
            continue;
        }
        if ((state & BR_SSL_RECVREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_recvrec_buf(m_engine, &bufLen);
            auto read = m_socket.recv(buf, bufLen);
            if (read < 0) {
                auto err = WiFiSocket::lastError();
                if (err == EWOULDBLOCK && received) {
                    return received;
                }
                s_lastError = err;
                return -1;
            }
            br_ssl_engine_recvrec_ack(m_engine, read);
            continue;
        }

        if (received)
            return received;

        s_lastError = WiFiBearSSLSocket::MustSend;
        return -1;
    }
}

bool WiFiBearSSLSocket::flush() {
    br_ssl_engine_flush(m_engine, false);
    return handshake();
}

bool WiFiBearSSLSocket::finish() {
    s_lastError = 0;
    br_ssl_engine_close(m_engine);
    while ( true ) {
        auto state = br_ssl_engine_current_state(m_engine);
        
        if (state == 0) {
            s_lastError = WiFiBearSSLSocket::NotInitialized;
            return false;
        }

        if (state == BR_SSL_CLOSED) {
            auto sslErr = br_ssl_engine_last_error(m_engine);
            if (sslErr != BR_ERR_OK) {
                s_lastError = WiFiBearSSLSocket::ProtocolError;
                return false;
            } 
            
            close();
            return true;
        }

        if ((state & BR_SSL_SENDREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_sendrec_buf(m_engine, &bufLen);
            auto sent = m_socket.send(buf, bufLen);
            if (sent < 0) {
                auto err = WiFiSocket::lastError();
                errno = err;
                return false;
            }
            br_ssl_engine_sendrec_ack(m_engine, sent);
            continue;
        }
        if ((state & BR_SSL_RECVREC) != 0) {
            size_t bufLen;
            auto buf = br_ssl_engine_recvrec_buf(m_engine, &bufLen);
            auto read = m_socket.recv(buf, bufLen);
            if (read < 0) {
                auto err = WiFiSocket::lastError();
                errno = err;
                return false;
            }
            br_ssl_engine_recvrec_ack(m_engine, read);
            continue;
        }

        //We shouldn't be here either, sigh
    }
}


#endif