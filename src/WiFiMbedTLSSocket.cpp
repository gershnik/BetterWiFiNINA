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

#include "WiFiMbedTLSSocket.h"

#if BETTER_WIFI_NINA_HAS_MBED_TLS

#include <errno.h>
#include <mbedtls/net.h>

int WiFiMbedTLSSocket::s_lastError = 0;

bool WiFiMbedTLSSocket::handshake() {
    s_lastError = mbedtls_ssl_handshake(m_context);
    return s_lastError >= 0;
}

int32_t WiFiMbedTLSSocket::send(const void * src, uint16_t size) {
    auto res = mbedtls_ssl_write(m_context, static_cast<const uint8_t *>(src), size);
    if (res >= 0) {
        s_lastError = 0;
        return res;
    }
    s_lastError = res;
    return -1;
}

int32_t WiFiMbedTLSSocket::recv(void * dest, uint16_t size) {
    auto res = mbedtls_ssl_read(m_context, static_cast<uint8_t *>(dest), size);
    if (res >= 0) {
        s_lastError = 0;
        return res;
    }
    s_lastError = res;
    return -1;
}

bool WiFiMbedTLSSocket::finish() {
    s_lastError = mbedtls_ssl_close_notify(m_context);
    if (s_lastError == 0) {
        m_socket.close();
        return true;
    }
    return false;
}


int WiFiMbedTLSSocket::sendCallback(void * ctx, const uint8_t * buf, size_t len) {
    auto socket = static_cast<WiFiSocket *>(ctx);
    auto sent = socket->send(buf, len);
    if (sent < 0) {
        auto err = WiFiSocket::lastError();
        if (err == EWOULDBLOCK)
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        if (err == ECONNRESET)
            return MBEDTLS_ERR_NET_CONN_RESET;
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }
    return sent;
}

int WiFiMbedTLSSocket::recvCallback(void * ctx, uint8_t * buf, size_t len) {
    auto socket = static_cast<WiFiSocket *>(ctx);
    auto read = socket->recv(buf, len);
    if (read < 0) {
        auto err = WiFiSocket::lastError();
        if (err == EWOULDBLOCK)
            return MBEDTLS_ERR_SSL_WANT_READ;
        if (err == ECONNRESET)
            return MBEDTLS_ERR_NET_CONN_RESET;
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }
    return read;
}

#endif
