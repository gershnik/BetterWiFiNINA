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
[rp2040-https]: https://gershnik.github.io/2024/04/03/https-arduino-nano-rp2040.html
[nina-root-cert]: https://support.arduino.cc/hc/en-us/articles/360016119219-Upload-SSL-root-certificates
[arduino-bear-ssl]: https://github.com/arduino-libraries/ArduinoBearSSL
[mbed-nano-tls]: https://github.com/gershnik/MbedNanoTLS

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

### Bear SSL wrapper over plain BSD sockets

[WiFiNINA][wifinina] lets you create SSL clients (but not servers!) via `WiFiClient`/`WiFiSSLClient` classes. This is convenient since you do not need any additional libraries but comes at a price of the aforementioned clunky interface and clunky process to [set up certificate trust][nina-root-cert]. On the other hand, using `WiFiSSLClient` makes SSL happen on Nina chip saving you significant amount of main board memory.

This library provides `WiFiBearSSLSocket` class that connects `WiFiSocket` with Bear SSL library and allows you to create either SSL clients or servers. You can most easily obtain Bear SSL via [ArduinoBearSSL][arduino-bear-ssl] library but other methods are also possible such as building it directly.

Note that running SSL (especially servers!) is RAM hungry. If your chip has only 32kB of RAM or less you might need to take extra precautions to avoid running out of memory. If your own code is already large then using `WiFiClient` for SSL and not having a server might be your only option.

The [WiFiBearSSLServer](examples/WiFiBearSSLServer) example demonstrates how to create an SSL server using `WiFiBearSSLSocket`.


### Mbed TLS wrapper over plain BSD sockets

If your Arduino board software is [Mbed OS based][mbedos-arduino] it is also possible to use `WiFiSocket` with [Mbed TLS][mbed-tls] which is the same library NINA firmware uses to provide SSL functionality. 

An older and slower version of it is included with your board library but you can also obtain latest and much faster one via [MbedNanoTLS][mbed-nano-tls] library.

BetterWiFiNINA provides `WiFiMbedTLSSocket` class that connects `WiFiSocket` with Mbed TLS and allows you to create either SSL clients or servers.

The [WiFiMbedTLSServer](examples/WiFiMbedTLSServer) example demonstrates how to create an SSL server using `WiFiMbedTLSSocket`.

## Docs and examples

Documentation for this library reflects the new APIs and is available at https://gershnik.github.io/BetterWiFiNINA/index.html. 

All relevant examples in this repository have been changed to use `WiFiSocket` instead of `WiFiClient`/`WiFiServer`/`WiFiUdp`.

## Integration

### Arduino IDE

- Download `BetterWiFiNINA-x.y.z.tar.gz` from the desired release of this library on 
[Releases](https://github.com/gershnik/BetterWiFiNINA/releases) page.
- Unpack it into your Arduino libraries folder. By default it is
  - macOS: `/Users/{username}/Documents/Arduino/libraries`
  - Linux: `/home/{username}/Arduino/libraries`
  - Windows: `C:\Users\{username}\Documents\Arduino\libraries`

### PlatformIO

Use either:

```ini
lib_deps =
    gershnik/BetterWiFiNINA@X.Y.Z
```

or

```ini
lib_deps =
    https://github.com/gershnik/BetterWiFiNINA#vX.Y.Z
```

to your `platformio.ini` file where X.Y.Z is the desired version.

Note that the second form allows you to reference unreleased code at specific branches or commits, if desired.

### Plain CMake

```cmake
include(FetchContent)

FetchContent_Declare(BetterWiFiNINA
    GIT_REPOSITORY  https://github.com/gershnik/BetterWiFiNINA
    GIT_TAG         vX.Y.Z
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(BetterWiFiNINA)

add_library(BetterWiFiNINA STATIC)

target_link_libraries(BetterWiFiNINA
PUBLIC
    SPI::SPI #replace with whatever name you use for SPI for your CMake setup
)

target_include_directories(BetterWiFiNINA 
PUBLIC
    ${betterwifinina_SOURCE_DIR}/src
)

file(GLOB_RECURSE BetterWiFiNINA_SOURCES ${betterwifinina_SOURCE_DIR}/src/*.cpp)
target_sources(BetterWiFiNINA
PRIVATE
    ${BetterWiFiNINA_SOURCES}
)

```


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



