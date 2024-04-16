Home
====

.. toctree::
   :maxdepth: 2
   :caption: Contents:
   :hidden:

   WiFi
   Socket
   BearSSLSocket
   MbedTLSSocket
   UDP
   Client
   Server


This library allows you to use WiFi capabilities of Arduino boards equipped with NINA module. Its source code can be found at 
`this Github page <https://github.com/gershnik/BetterWiFiNINA>`_

It is a fork of `WiFiNINA <https://github.com/arduino-libraries/WiFiNINA>`_ library that attempts to improve it and fix longstanding issues
to make it usable for more serious network communication needs.

The library supports WEP, WPA2 Personal and WPA2 Enterprise encryptions. 

To use this library:

.. highlight:: cpp

::

   #include <SPI.h>
   #include <BetterWiFiNINA.h>
   


The following pages describe various aspects of the library functionality:

* :doc:`WiFi` - connect to/establish a WiFi network and various global functionality
* :doc:`Socket` - a safe interface to BSD-style sockets. Use this interface if possible.
* :doc:`BearSSLSocket` - using sockets with Bear SSL. Use this interface if possible.
* :doc:`MbedTLSSocket` - using sockets with Mbed TLS. Use this interface if possible.
* :doc:`UDP` - a simplified interface to UDP. 
* :doc:`Client` - a simplified interface to plain TCP and SSL clients. Only useful for client SSL connections and otherwise deprecated.
* :doc:`Server` - don't use

Various examples can be found `here <https://github.com/gershnik/BetterWiFiNINA/tree/mainline/examples>`_.


Indices and info
==================

* :ref:`genindex`
* :ref:`search`

License
=======

| Copyright (c) 2024 Eugene Gershnik. All rights reserved.
| Copyright (c) 2018 Arduino SA. All rights reserved.
| Copyright (c) 2011-2014 Arduino LLC. All right reserved.
| 
| This library is free software; you can redistribute it and/or modify it under
| the terms of the GNU Lesser General Public License as published by the Free
| Software Foundation; either version 3 of the License, or (at your option)
| any later version.
|
| This library is distributed in the hope that it will be useful, but
| WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
| for more details.
|  
| You should have received a copy of the GNU Lesser General Public License along
| with this library; if not, see <https://www.gnu.org/licenses/>.

