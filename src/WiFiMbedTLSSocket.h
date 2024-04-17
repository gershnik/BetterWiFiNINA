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

#ifndef HEADER_WIFI_MBED_TLS_SOCKET_H_INCLUDED
#define HEADER_WIFI_MBED_TLS_SOCKET_H_INCLUDED


#if __has_include(<mbedtls/ssl.h>)
    #include <mbedtls/ssl.h>
    #define BETTER_WIFI_NINA_HAS_MBED_TLS 1
#endif

#if BETTER_WIFI_NINA_HAS_MBED_TLS

#include "WiFiSocket.h"

/**
 * A wrapper over a socket that adds SSL using Mbed TLS
 * 
 * This class is only available if Mbed TLS is present during the compilation.
 * It is detected via presence of `<mbedtls/ssl.h>` header.
 * Mbed TLS is available on Mbed OS based Nano boards and you can also obtain
 * a newer and faster version via [MbedNanoTLS](https://github.com/gershnik/MbedNanoTLS)
 * library
 * 
 * This class takes a pre-existing socket obtained from elsewhere 
 * and assumes ownership over it. Like the original socket it is movable and
 * move assignable but not copyable or copy assignable.
 * 
 * Both blocking and non-blocking sockets are supported.
*/
class WiFiMbedTLSSocket {
public:
    /**
     * Retrieves error (if any) of the last method call.
     * 
     * Last error is always set, whether the call failed or succeeded.
     * 
     * @returns one of the MBed TLS error codes. 
    */
    static int lastError()
        { return s_lastError; }

    /// Creates an invalid socket
    WiFiMbedTLSSocket() = default;

    /**
     * Creates a socket
     * 
     * This method never fails. The source socket should be in connected state
     * and ready to use - you will not be able to access it after this call.
     * 
     * @param socket the plain socket to assume ownership of
     * @param context Mbed TLS session context. 
    */
    WiFiMbedTLSSocket(WiFiSocket && socket, mbedtls_ssl_context * context):
        m_socket(static_cast<WiFiSocket &&>(socket)),
        m_context(context)
    {
        mbedtls_ssl_set_bio(m_context, &m_socket, &WiFiMbedTLSSocket::sendCallback, &WiFiMbedTLSSocket::recvCallback, nullptr);
    }

    /**
     * Moving a socket
     * 
     * The source socket is left in an invalid state
    */
    WiFiMbedTLSSocket(WiFiMbedTLSSocket && src): 
        m_socket(static_cast<WiFiSocket &&>(src.m_socket)),
        m_context(src.m_context)
    { 
        if (m_context) {
            src.m_context = nullptr;
            mbedtls_ssl_set_bio(m_context, &m_socket, sendCallback, recvCallback, nullptr);
        }
    }

    /**
     * Move-assigning a socket
     * 
     * The source socket is left in an invalid state
    */
    WiFiMbedTLSSocket & operator=(WiFiMbedTLSSocket && src) {
        if (this != &src) {
            m_socket = static_cast<WiFiSocket &&>(src.m_socket);
            m_context = src.m_context;
            if (m_context) {
                src.m_context = nullptr;
                mbedtls_ssl_set_bio(m_context, &m_socket, sendCallback, recvCallback, nullptr);
            }
        }
        return *this;
    }

    /**
     * Tests whether the socket is invalid.
     * 
     * A socket is in an invalid state when it represents "no socket".
     * A valid socket never becomes invalid unless it is moved out or closed. 
     * Similarly an invalid socket never becomes valid
     * unless moved-in from a valid socket.
     * 
    */
    explicit operator bool() const 
        { return bool(m_socket); }

    /**
     * Manually close the socket
     * 
     * This makes this object an invalid socket.
     * Note that this method does NOT gracefully close SSL connection.
     * It just brute-force closes the socket.
     * Call finish() to gracefully close.
    */ 
    void close() {
        m_socket.close();
        m_context = nullptr;
    }

    /**
     * Perform an SSL handshake.
     * 
     * @returns success flag. Check lastError() for more information about failure
    */
    bool handshake();

    /**
     * Sends data to remote endpoint
     * 
     * @return the amount of data actually sent or -1 on failure. Check lastError() 
     * for more information about failure. The type of the return value is int32_t 
     * to accommodate -1. When non-negative it will never be bigger than the size parameter. 
    */
    int32_t send(const void * buf, uint16_t size);

    /**
     * Receives data from remote endpoint
     * 
     * @return the amount of data actually read or -1 on failure. Check lastError() 
     * for more information about failure. The type of the return value is int32_t 
     * to accommodate -1. When non-negative it will never be bigger than the size parameter.
    */
    int32_t recv(void * buf, uint16_t size);

    /**
     * Does nothing
     * 
     * This function is provided for compatibility with other SSL implementations
     * that might require you to manually flush buffered data.
     * 
     * @returns true
    */
    bool flush() {
        s_lastError = 0;
        return true;
    }

    /**
     * Gracefully closes SSL connection
     * 
     * When this method returns `true` the underlying socket is also closed.
     * For non-blocking sockets it might return MBEDTLS_ERR_SSL_WANT_READ/WRITE so be prepared to 
     * call it multiple times.
     * 
     * @returns success flag. Check lastError() for more information about failure
    */
    bool finish();

private:
    static int sendCallback(void * ctx, const uint8_t * buf, size_t len);
    static int recvCallback(void * ctx, uint8_t * buf, size_t len);
private:
    WiFiSocket m_socket;
    mbedtls_ssl_context * m_context = nullptr;

    static int s_lastError;
};

#endif

#endif