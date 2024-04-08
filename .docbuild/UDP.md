# UDP

`WiFiUDP` class provides a simplified interface to UDP. While still supported in this library, all of its functionality and more can be accomplished using [WiFiSocket](Socket.rst) class instead.

## WiFiUDP class
```{eval-rst}
.. index:: WiFiUDP (C++ class)
```

### `WiFiUDP`
```{eval-rst}
.. index:: WiFiUDP::WiFiUDP (C++ function)
```

#### Description
Creates a named instance of the WiFi UDP class that can send and receive UDP messages.

#### Syntax

```
WiFiUDP
```

#### Parameters
none

### `WiFiUDP.begin()`
```{eval-rst}
.. index:: WiFiUDP::begin (C++ function)
```

#### Description
Initializes the WiFi UDP library and network settings. Starts WiFiUDP socket, listening at local port PORT.

#### Syntax

```
WiFiUDP.begin(port)
```

#### Parameters

- port: the local port to listen on (int)

#### Returns

- 1: if successful
- 0: if there are no sockets available to use

### `WiFiUDP.beginMulticast()`
```{eval-rst}
.. index:: WiFiUDP::beginMulticast (C++ function)
```

#### Description

Initializes the WiFi UDP library and network settings. Starts WiFiUDP socket, listening at local port PORT and on specified multicast IP address.

#### Syntax

```
WiFiUDP.beginMulticast(IPAddress, port);
```

#### Parameters

- IPAddress: sdd
- port: the local port to listen on (int)

#### Returns

- 1: if successful
- 0: if there are no sockets available to use

#### Example

```
…
WiFiUDP udp;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
        ;
  }

  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");

        while (true);
  }

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");

  udp.beginMulticast(IPAddress(226, 1, 1, 1), 4096);
  Serial.println("\n connected UDP multicast");

…
```

### `WiFiUDP.beginPacket()`
```{eval-rst}
.. index:: WiFiUDP::beginPacket (C++ function)
```

#### Description
Starts a connection to write UDP data to the remote connection

#### Syntax

```
WiFiUDP.beginPacket(hostName, port);
WiFiUDP.beginPacket(hostIp, port);

```

#### Parameters

- hostName: the address of the remote host. It accepts a character string or an IPAddress
- hostIp: the IP address of the remote connection (4 bytes)
- port: the port of the remote connection (int)

#### Returns

- 1: if successful
- 0: if there was a problem with the supplied IP address or port


### `WiFiUDP.write()`
```{eval-rst}
.. index:: WiFiUDP::write (C++ function)
```

#### Description
Writes UDP data to the remote connection. Must be wrapped between beginPacket() and endPacket(). beginPacket() initializes the packet of data, it is not sent until endPacket() is called.

#### Syntax

```
WiFiUDP.write(byte);
WiFiUDP.write(buffer, size);

```

#### Parameters

- byte: the outgoing byte
- buffer: the outgoing message
- size: the size of the buffer

#### Returns

- single byte into the packet
- bytes size from buffer into the packet

### `WiFiUDP.endPacket()`
```{eval-rst}
.. index:: WiFiUDP::endPacket (C++ function)
```

#### Description

Called after writing UDP data to the remote connection. It finishes off the packet and send it.

#### Syntax

```
WiFiUDP.endPacket()
```

#### Parameters

- None

#### Returns

- 1: if the packet was sent successfully
- 0: if there was an error

### `WiFiUDP.available()`
```{eval-rst}
.. index:: WiFiUDP::available (C++ function)
```

#### Description
Get the number of bytes (characters) available for reading from the buffer. This is data that's already arrived.

This function can only be successfully called after WiFiUDP.parsePacket().

available() inherits from the Stream utility class.

#### Syntax

```
WiFiUDP.available()

```

#### Parameters

- None

#### Returns

- the number of bytes available in the current packet
- 0: if WiFiUDP.parsePacket() hasn't been called yet

### `WiFiUDP.parsePacket()`
```{eval-rst}
.. index:: WiFiUDP::parsePacket (C++ function)
```

#### Description

It starts processing the next available incoming packet, checks for the presence of a UDP packet, and reports the size. parsePacket() must be called before reading the buffer with UDP.read().

#### Syntax

```
WiFiUDP..parsePacket()
```

#### Parameters
- None

#### Returns
- the size of the packet in bytes
- 0: if no packets are available

### `WiFiUDP.peek()`
```{eval-rst}
.. index:: WiFiUDP::peek (C++ function)
```

#### Description
Read a byte from the file without advancing to the next one. That is, successive calls to peek() will return the same value, as will the next call to read().

This function inherited from the Stream class. See the Stream class main page for more information.

#### Syntax

```
WiFiUDP.peek()

```

#### Parameters

- None

#### Returns
- b: the next byte or character
- -1: if none is available

### `WiFiUDP.read()`
```{eval-rst}
.. index:: WiFiUDP::read (C++ function)
```

#### Description
Reads UDP data from the specified buffer. If no arguments are given, it will return the next character in the buffer.

This function can only be successfully called after WiFiUDP.parsePacket().

#### Syntax

```
WiFiUDP.read();
WiFiUDP.read(buffer, len);
```

#### Parameters
- buffer: buffer to hold incoming packets (char*)
len: maximum size of the buffer (int)

#### Returns
- b: the characters in the buffer (char)
- size: the size of the buffer
- -1: if no buffer is available

### `WiFiUDP.flush()`
```{eval-rst}
.. index:: WiFiUDP::flush (C++ function)
```

#### Description
Discard any bytes that have been written to the client but not yet read.

`flush()` inherits from the Stream utility class.

#### Syntax

```
WiFiUDP.flush()
```

#### Parameters
none

#### Returns
- None

### `WiFiUDP.stop()`
```{eval-rst}
.. index:: WiFiUDP::stop (C++ function)
```

#### Description
Disconnect from the server. Release any resource being used during the UDP session.

#### Syntax

```
WiFiUDP.stop()
```

#### Parameters
- None

#### Returns
- None

### `WiFiUDP.remoteIP()`
```{eval-rst}
.. index:: WiFiUDP::remoteIP (C++ function)
```

#### Description
Gets the IP address of the remote connection.

This function must be called after `WiFiUDP.parsePacket()`.

#### Syntax

```
WiFiUDP.remoteIP();
```

#### Parameters

- None

#### Returns

- 4 bytes : the IP address of the host who sent the current incoming packet


### `WiFiUDP.remotePort()`
```{eval-rst}
.. index:: WiFiUDP::remotePort (C++ function)
```

#### Description
Gets the port of the remote UDP connection.

This function must be called after UDP.parsePacket().

#### Syntax

```
UDP.remotePort()
```

#### Parameters

- None

#### Returns

- The port of the host who sent the current incoming packet

