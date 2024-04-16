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

#ifndef HEADER_WIFI_BEAR_SSL_SOCKET_H_INCLUDED
#define HEADER_WIFI_BEAR_SSL_SOCKET_H_INCLUDED

#if __has_include(<bearssl/bearssl.h>)
    #include <bearssl/bearssl.h>
    #define BETTER_WIFI_NINA_HAS_BEAR_SSL 1
#elif __has_include(<bearssl.h>)
    #include <bearssl.h>
    #define BETTER_WIFI_NINA_HAS_BEAR_SSL 1
#endif

#if BETTER_WIFI_NINA_HAS_BEAR_SSL

#include "WiFiSocket.h"

/**
 * A wrapper over a socket that adds SSL using Bear SSL
 * 
 * This class is only available if Bear SSL is present during the compilation.
 * It is detected via presence of either `<bearssl/bearssl.h>` or `<bearssl.h>`
 * header.
 * You can most easily obtain Bear SSL via ArduinoBearSSL library.
 * 
 * This class takes a pre-existing socket obtained from elsewhere 
 * and assumes ownership over it. Like the original socket it is movable and
 * move assignable but not copyable or copy assignable.
 * 
 * Both blocking and non-blocking sockets are supported.
*/
class WiFiBearSSLSocket {
public:
    /**
     * Error codes specific to this class. These may be returned from
     * WiFiBearSSLSocket::lastError(). 
     * 
     * Their range is such that it will not overlap with plain socket errors.
    */
    enum Error {
        /// `br_ssl_engine_context` is not fully initialized yet
        NotInitialized = -1,
        /// SSL protocol error, call `br_ssl_engine_last_error()` on the engine to find out more 
        ProtocolError = -2,
        /// Cannot send() now. You must recv() first
        MustRecv = -3,
        /// Cannot recv() now. You must send() first
        MustSend = -4,
    };
public:
    /**
     * Retrieves error (if any) of the last method call.
     * 
     * Last error is always set, whether the call failed or succeeded.
     * 
     * @returns either a WiFiSocket::lastError() code (if the underlying socket
     * operation failed) or one of the SSLError error codes. The ranges of these
     * errors are disjoint.
    */
    static int lastError()
        { return s_lastError; }

    /// Creates an invalid socket
    WiFiBearSSLSocket() = default;

    /**
     * Creates a socket
     * 
     * This method never fails. The source socket should be in connected state
     * and ready to use - you will not be able to access it after this call.
     * 
     * @param socket the plain socket to assume ownership of
     * @param eng Bear SSL engine context. 
    */
    WiFiBearSSLSocket(WiFiSocket && socket, br_ssl_engine_context * eng):
        m_socket(static_cast<WiFiSocket &&>(socket)),
        m_engine(eng)
    {}

    /**
     * Moving a socket
     * 
     * The source socket is left in an invalid state
    */
    WiFiBearSSLSocket(WiFiBearSSLSocket && src): 
        m_socket(static_cast<WiFiSocket &&>(src.m_socket)),
        m_engine(src.m_engine)
    { 
        src.m_engine = nullptr;
    }

    /**
     * Move-assigning a socket
     * 
     * The source socket is left in an invalid state
    */
    WiFiBearSSLSocket & operator=(WiFiBearSSLSocket && src) {
        if (this != &src) {
            m_socket = static_cast<WiFiSocket &&>(src.m_socket);
            m_engine = src.m_engine;
            src.m_engine = nullptr; 
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
        m_engine = nullptr;
    }

    /**
     * Perform an SSL handshake.
     * 
     * Due to Bear SSL design, this method is not strictly necessary. If you don't call
     * it and just start using send() or recv() the handshake will be performed automatically 
     * for you. However, it is sometime convenient to isolate handshake stage in its own step
     * so this method is provided for convenience and compatibility with other libraries. 
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
     * Sends any buffered data to the remote endpoint
     * 
     * Bear SSL buffers the data you send() and doesn't necessarily immediately send it 
     * over the network. This method allows you to send any buffered data immediately. 
     * 
     * @returns success flag. Check lastError() for more information about failure
    */
    bool flush();

    /**
     * Gracefully closes SSL connection
     * 
     * When this method returns `true` the underlying socket is also closed.
     * For non-blocking sockets it might return EWOULDBLOCK so be prepared to call it
     * multiple times.
     * 
     * @returns success flag. Check lastError() for more information about failure
    */
    bool finish();
private:
    WiFiSocket m_socket;
    br_ssl_engine_context * m_engine = nullptr;

    static int s_lastError;
};

#endif

#endif