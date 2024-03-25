# BetterWiFiNINA

[![Check Arduino status](https://github.com/gershnik/BetterWiFiNina/actions/workflows/check-arduino.yml/badge.svg)](https://github.com/gershnik/BetterWiFiNina/actions/workflows/check-arduino.yml)
[![Compile Examples status](https://github.com/gershnik/BetterWiFiNina/actions/workflows/compile-examples.yml/badge.svg)](https://github.com/gershnik/BetterWiFiNina/actions/workflows/compile-examples.yml)
[![Spell Check status](https://github.com/gershnik/BetterWiFiNina/actions/workflows/spell-check.yml/badge.svg)](https://github.com/gershnik/BetterWiFiNina/actions/workflows/spell-check.yml)


<!-- Links -->
[wifinina]: https://github.com/arduino-libraries/WiFiNINA
[wifinina-reference]: https://www.arduino.cc/reference/en/libraries/wifinina/
[crosstalk]: https://github.com/arduino-libraries/WiFiNINA/issues/176
[no-client-and-server]: https://www.reddit.com/r/arduino/comments/exc3hd/wifinina_server_and_client_simultaneously/
[mbedos-arduino]: https://docs.arduino.cc/software/ide-v1/tutorials/getting-started/cores/arduino-mbed_nano
[mbed-tls]: https://www.trustedfirmware.org/projects/mbed-tls/

<!-- End Links -->

This is a fork of [WiFiNINA][wifinina] library that attempts to improve it and fix longstanding issues
to make it usable for more serious network communication needs.

Currently it **does not** require any custom NINA firmware. Regular NINA firmware 1.5 or above will work.
This may or may not change in the future.

In the spirit of open source all notable changes in this library will be submitted for inclusion to WiFiNINA.
Whether or not that will happen (and when) is up to WiFiNINA maintainers.

For more information about original WiFiNINA library please visit [WiFiNINA Reference page][wifinina-reference]

## Functionality added

### Plain BSD sockets

[WiFiNINA][wifinina] exposes a "simplified" interface for network communications via its `WiFiClient`/`WiFiServer` classes
that tries to hide the underlying socket operations complexity and manage them for you. Unfortunately this interface
is riddled with issues and longstanding problems (e.g. see [this][crosstalk] and [this][no-client-and-server]) that 
seem to be very hard or impossible to fix.

Fortunately, since version 1.5 NINA firmware exposes direct access to BSD sockets which makes it possible to expose them
to client code. This is precisely what this library does. It exposes a new class `WiFiSocket` that provides type-safe
access to normal BSD socket functionality: `socket`/`bind`/`listen`/`accept`/`connect`/`recv`/`send` etc.

Notably non-blocking sockets are supported, enabling your code to easily handle multiple connections and multiple 
servers and clients.

If you are familiar with a regular BSD socket API this interface should provide a far better and easier experience than
using `WiFiClient`/`WiFiServer`.

#### But what about SSL?

One thing `WiFiClient`/`WiFiServer` interface provides that `WiFiSocket` doesn't, is a transparent ability to perform 
**client-only** SSL communication. The facilities to do so are baked into NINA firmware and currently are not directly 
exposed the way plain sockets are. Client-only means that you can connect to SSL servers as a client but not run your own 
server. It also must be mentioned that to manage SSL certificate trust you need to go through a cumbersome process of uploading
trusted root certificates to the NINA chip.

Fortunately, as long as your Arduino board software is [Mbed OS based][mbedos-arduino] there is usually a better way. Mbed OS includes 
[Mbed TLS][mbed-tls] which is the same library NINA firmware uses to provide SSL functionality. It is relatively straightforward
to integrate Mbed TLS (via custom BIO) with `WiFiSocket` and voila - you now have an ability to run *both* SSL servers and clients
with certificates and trust under your full control. A document describing how to do it in more detail is upcoming.

If your board is not Mbed OS based then, as far as I know, you are out of luck and using `WiFiSSLClient` with all its limitations
is your only option.

## Docs and examples

The docs and examples for this library are still the ones from WiFiNINA. Updating/converting them is a work in progress


## License

Copyright (c) 2024 Eugene Gershnik. All rights reserved.<br>
Copyright (c) 2018 Arduino SA. All rights reserved.<br>
Copyright (c) 2011-2014 Arduino LLC. All right reserved.<br>

This library is free software; you can redistribute it and/or modify it under<br>
the terms of the GNU Lesser General Public License as published by the Free<br>
Software Foundation; either version 3 of the License, or (at your option)<br>
any later version.

This library is distributed in the hope that it will be useful, but<br>
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or<br>
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License<br>
for more details.
  
You should have received a copy of the GNU Lesser General Public License along<br>
with this library; if not, see <https://www.gnu.org/licenses/>.



