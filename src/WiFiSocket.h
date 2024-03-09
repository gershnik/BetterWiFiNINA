/*
  This file is part of the WiFiNINA library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  Created on: Mar 6, 2024
  Author: gershnik
*/

#ifndef HEADER_WIFISOCKET_H_INCLUDED
#define HEADER_WIFISOCKET_H_INCLUDED

#include <stdint.h>

#include <IPAddress.h>

#include "utility/socket_drv.h"


/**
 * A plain socket
 * 
 * This class owns the underlying socket lifetime. It is movable and
 * move assignable but not copyable or copy assignable.
*/
class WiFiSocket {
private:
    static constexpr uint8_t s_invalidHandle = 255;
public:
    /**
     * Socket type
     * 
     * Semantics equivalent to Unix sockets SOCK_STREAM, SOCK_DGRAM etc.
    */
    enum class Type: uint8_t {
        //see lwip/sockets.h SOCK_STREAM etc. for values
        Stream  = 1,
        DGram   = 2,
        Raw     = 3
    };

    /**
     * Socket protocol
     * 
     * Semantics equivalent to Unix sockets IPPROTO_TCP etc.
     * Not all values are implemented/supported in firmware
    */
    enum Protocol : uint8_t {
        //see lwip/sockets.h IPPROTO_IP etc.
        IP      = 0,
        ICMP    = 1,
        TCP     = 6,
        UDP     = 17,
        IPv6    = 41, 
        ICMPv6  = 58,
        UDPLite = 136,
        Raw     = 255
    };

    /**
     * Socket state bitmask
     * 
     * These values can be obtained via poll()
    */
    enum State : uint8_t {
        Readable    = SocketDrv::SocketReadable,
        Writable    = SocketDrv::SocketWritable,
        ErroredOut  = SocketDrv::SocketErroredOut
    };

    /**
     * Custom error codes for lastError()
     * 
     * These represent issues communicating with NINA chip
     * and won't clash with errno codes returned from 
     * socket calls.
    */
    enum class Error : uint8_t {
        SpiFailure = SocketDrv::Failure
    };

private:
    enum class IOControl : uint32_t {
        //see lwip/sockets.h
        NRead           = 0x4004667F, //FIONREAD
        NonBlockingIO   = 0x8004667E  //FIONBIO
    };
public:
    /**
     * Retrieves error (if any) of the last method call
     * 
     * Last error is always set, whether the call failed or succeeded.
     * 
     * The returned value is either a standard errno value from
     * the underlying socket call or one of the Error enumeration values
     * Their ranges are guaranteed to be distinct.
     * In case of success the value is 0.
    */
    static uint8_t lastError();

    /// Creates an invalid socket
    WiFiSocket() = default;

    /**
     * Creates a socket
     * 
     * This is equivalent to socket() API call.
     * In case of failure the socket is created as invalid which
     * can be tested via operator bool.
    */
    WiFiSocket(Type type, Protocol proto);

    /// Closes the socket
    ~WiFiSocket() {
        close();
    }

    /**
     * Manually close the socket
     * 
     * This makes this object an invalid socket
    */ 
    void close();

    /**
     * Moving a socket
     * 
     * The source socket is left in an invalid state
    */
    WiFiSocket(WiFiSocket && src): m_handle(src.m_handle) { 
        src.m_handle = s_invalidHandle; 
    }

    /**
     * Move-assigning a socket
     * 
     * The source socket is left in an invalid state
    */
    WiFiSocket & operator=(WiFiSocket && src) {
        if (this != &src) {
            close();
            m_handle = src.m_handle;
            src.m_handle = s_invalidHandle; 
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
        { return m_handle != s_invalidHandle; }

    /**
     * Binds a socket to given port
     * @returns success flag. Check lastError() for more information about failure
    */
    bool bind(uint16_t port);

    /**
     * Starts listening for incoming connections
     * @returns success flag. Check lastError() for more information about failure
    */
    bool listen(uint8_t backlog);

    /**
     * Accepts an incoming connection
     * 
     * @returns a valid socket, if successful or invalid otherwise. Check lastError()
     * for more information about the failure.  
     * 
     * @param remoteIpAddress if successful populated by the address of the remote client
     * @param remotePort if successful populated by the port of the remote client
    */
    WiFiSocket accept(arduino::IPAddress & remoteIpAddress, uint16_t & remotePort);

    /**
     * Connects a socket to remote endpoint
     * 
     * @returns success flag. Check lastError() for more information about failure
     * @param remoteIpAddress host to connect to
     * @param remotePort port to connect to
    */
    bool connect(const arduino::IPAddress & remoteIpAddress, uint16_t remotePort);


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
     * Sets the socket into non-blocking or blocking mode
     * 
     * This is equivalent to ioctl(...FIONBIO...)
     * 
     * @returns success flag. Check lastError() for more information about failure
    */
    bool setNonBlocking(bool val);

    /** 
     * Retrieves the number of bytes available for reading.
     * 
     * This is equivalent to ioctl(...FIONREAD...)
     * 
     * @returns success flag. Check lastError() for more information about failure
    */
    int32_t availableToRead() const;

    /**
     * Retrieves current socket state bitmask
     * 
     * See State enum for possible flags. This call is similar in semantics to 
     * calling select() on the socket.
     * 
     * @returns success flag. Check lastError() for more information about failure
     * 
    */
    bool poll(State & state) const;

    /**
     * Retrieves underlying socket handle
     * 
     * This is for debugging purposes only. 
    */
    uint8_t handle() const 
        { return m_handle; }
private:
    explicit WiFiSocket(uint8_t handle): m_handle(handle)
    {}
private:
    uint8_t m_handle = s_invalidHandle;
};

#endif