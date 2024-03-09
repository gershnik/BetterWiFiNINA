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

#include "WiFiSocket.h"


uint8_t WiFiSocket::lastError() {
    return SocketDrv::lastError();
}


WiFiSocket::WiFiSocket(Type type, Protocol proto):
    m_handle(SocketDrv::socket(uint8_t(type), uint8_t(proto)))
{
}

void WiFiSocket::close() {
    if (m_handle != s_invalidHandle) {
        SocketDrv::close(m_handle);
        m_handle = s_invalidHandle;
    }
}

bool WiFiSocket::bind(uint16_t port) {
    return SocketDrv::bind(m_handle, port);
}

bool WiFiSocket::listen(uint8_t backlog) {
    return SocketDrv::listen(m_handle, backlog);
}

WiFiSocket WiFiSocket::accept(arduino::IPAddress & remoteIpAddress, uint16_t & remotePort) {
    auto res = SocketDrv::accept(m_handle, remoteIpAddress, remotePort);
    return WiFiSocket(res);
}

bool WiFiSocket::connect(const arduino::IPAddress & remoteIpAddress, uint16_t remotePort) {
    return SocketDrv::connect(m_handle, remoteIpAddress, remotePort);
}

int32_t WiFiSocket::send(const void * buf, uint16_t size) {
    auto ret = SocketDrv::send(m_handle, buf, size);
    if (ret == 0 && lastError() != 0)
        ret = -1;
    return ret;
}

int32_t WiFiSocket::recv(void * buf, uint16_t size) {
    auto ret = SocketDrv::recv(m_handle, buf, size);
    if (ret == 0 && lastError() != 0)
        ret = -1;
    return ret;
}

bool WiFiSocket::setNonBlocking(bool val) {
    uint32_t value = val;
    auto size = SocketDrv::ioctl(m_handle, uint32_t(IOControl::NonBlockingIO), 
                                 &value, sizeof(value));
    return size != 0;
}

int32_t WiFiSocket::availableToRead() const {
    int32_t value = 0;
    auto size = SocketDrv::ioctl(m_handle, uint32_t(IOControl::NRead), 
                                 &value, sizeof(value));
    if (size == 0)
        return -1;
    return value;
}

bool WiFiSocket::poll(State & state) const {
    auto res = SocketDrv::poll(m_handle);
    if (res == SocketDrv::SocketPollFailed)
        return false;
    state = State(res);
    return true;
}