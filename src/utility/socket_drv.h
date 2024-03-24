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

#ifndef HEADER_UTILITY_SOCKET_DRV_H_INCLUDED
#define HEADER_UTILITY_SOCKET_DRV_H_INCLUDED

#include <stdint.h>

#include <IPAddress.h>

class IPAddress;

namespace SocketDrv {

    enum CommunicationError : uint8_t {
        Failure = 255
    };

    enum SocketState : uint8_t {
        SocketReadable       =   0x01,
        SocketWritable       =   0x02,
        SocketErroredOut     =   0x04,
        SocketPollFailed     =   0x80
    };

    uint8_t socket(uint8_t type, uint8_t proto);
    bool close(uint8_t s);
    uint8_t lastError();
    bool bind(uint8_t s, uint16_t port);
    bool listen(uint8_t s, uint8_t backlog);
    uint8_t accept(uint8_t s, arduino::IPAddress & remoteIpAddress, uint16_t & remotePort);
    bool connect(uint8_t s, const arduino::IPAddress & ipAddress, uint16_t port);
    int32_t send(uint8_t s, const void * buf, uint16_t size);
    int32_t recv(uint8_t s, void * buf, uint16_t size);
    int32_t sendTo(uint8_t s, const void * buf, uint16_t size, const arduino::IPAddress & ipAddress, uint16_t port);
    int32_t recvFrom(uint8_t s, void * buf, uint16_t size, arduino::IPAddress & remoteIpAddress, uint16_t & remotePort);
    uint8_t ioctl(uint8_t s, uint32_t code, void * buf, uint8_t bufSize);
    SocketState poll(uint8_t s);
    bool setsockopt(uint8_t s, uint32_t optionName, const void * option, uint8_t optLen);
    bool getsockopt(uint8_t s, uint32_t optionName, void * option, uint8_t & optLen);
    bool getPeerName(uint8_t s, arduino::IPAddress & remoteIpAddress, uint16_t & remotePort);
}


#endif