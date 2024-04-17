# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),

## Unreleased

## [1.3.0] - 2024-04-16

### Added
- `WiFiMbedTLSSocket` class to allow SSL over plain sockets using Mbed TLS.
- `WiFiMbedTLSServer` sample that demonstrates an SSL web server over plain sockets using Mbed TLS

### Fixed
- Added missing move constructor to `WiFiBearSSLSocket`
- Made `WiFiBearSSLSocket::lastError()` correctly report 0 on success.

## [1.2.0] - 2024-04-16

### Added
- `WiFiBearSSLSocket` class to allow SSL over plain sockets using Bear SSL.
- `WiFiBearSSLServer` sample that demonstrates an SSL web server over plain sockets using Bear SSL

## [1.1.0] - 2024-04-07

### Added
- This library can now be used with PlatformIO
- Documentation has been updated and available now at https://gershnik.github.io/BetterWiFiNINA/index.html

### Changed
- All relevant examples have been changed to use `WiFiSocket` instead of `WiFiClient`/`WiFiServer`/`WiFiUdp`

## [1.0.0] - 2024-03-24

### Added
- Initial version

[1.0.0]: https://github.com/gershnik/BetterWiFiNINA/releases/v1.0.0
[1.1.0]: https://github.com/gershnik/BetterWiFiNINA/releases/v1.1.0
[1.2.0]: https://github.com/gershnik/BetterWiFiNINA/releases/v1.2.0
[1.3.0]: https://github.com/gershnik/BetterWiFiNINA/releases/v1.3.0
