# Client

The `Client` class hierarchy provides a simplified interface to plain TCP and SSL clients. 

`WiFiClient` is mostly obsolete as all of its functionality and more can be accomplished using [WiFiSocket](Socket.rst), [WiFiBearSSLSocket](BearSSLSocket.rst) or [WiFiMbedTLSSocket](MbedTLSSocket.rst) class instead.

`WiFiSSLClient` may still be useful to perform SSL client connections without external SSL libraries.

## Client Class

### `Client()`
```{eval-rst}
.. index:: Client (C++ class)
```

#### Description

Client is the base class for all WiFi client based calls. It is not called directly, but invoked whenever you use a function that relies on it.

### `WiFiClient()`
```{eval-rst}
.. index:: WiFiClient (C++ class)
```

#### Description

Creates a client that can connect to to a specified internet IP address and port as defined in client.connect().

#### Syntax

```
WiFiClient client;
```

#### Parameters

- client : the named client to refer to

#### Returns

- None

#### Example

```
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
IPAddress server(74,125,115,105);  // Google

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {

}
```

### `WiFiSSLClient`
```{eval-rst}
.. index:: WiFiSSLClient (C++ class)
```

#### Description
This class allows to create a client that always connects in SSL to the specified IP address and port, even if client.connect() is used instead of client.connectSSL(). This is useful If you have a library that accepts only plain Client, but you want to force it to use SSL, keeping the same method names of the non SSL client.

#### Syntax

```
WiFiNINASSLClient client;

```

#### Parameters

- client : the named client to refer to

#### Return

- None

#### Example

```
/*
This example creates a client object that connects and transfers
data using always SSL.

It is compatible with the methods normally related to plain
connections, like client.connect(host, port).

Written by Arturo Guadalupi
last revision November 2015

*/

#include <SPI.h>
#include <BetterWiFiNINA.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWiFiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
```

### `client.connected()`
```{eval-rst}
.. index:: Client::connected (C++ function)
```

#### Description
Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread data.

#### Syntax

```
client.connected()

```

#### Parameters

- None

#### Returns

- Returns true if the client is connected, false if not.

#### Example

```
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
IPAddress server(74,125,115,105);  // Google

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {
   if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}
```

### `client.connect()`
```{eval-rst}
.. index:: Client::connect (C++ function)
```

#### Description
Connect to the IP address and port specified in the constructor. The return value indicates success or failure. connect() also supports DNS lookups when using a domain name (e.g., google.com).

#### Syntax

```
client.connect(ip, port)
client.connect(URL, port)

```

#### Parameters

- ip: the IP address that the client will connect to (array of 4 bytes)
- URL: the domain name the client will connect to (string e.g., "arduino.cc")
- port: the port that the client will connect to (int)

#### Returns

- Returns true if the connection succeeds, false if not.

#### Example

```
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
char servername[]="google.com";  // remote server we will connect to

WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(servername, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {

}
```

### `client.connectSSL()`
```{eval-rst}
.. index:: Client::connectSSL (C++ function)
```

#### Description

Connect to the IP address and port specified in the constructor using the SSL protocol. The method connectSSL is required when the server provides only HTTPS connections. Before using this method, it is required to load the SSL certificate used by the server into the Arduino WiFi module . The boards come already loaded with certificates and it should be ready to use. To change or upload new SSL certificates you should follow the procedures that will be made available. connectSSL() also supports DNS lookups when using a domain name (e.g., google.com).

#### Syntax

```
client.connectSSL(ip, port)
client.connectSSL(URL, port)

```

#### Parameters

- ip: the IP address that the client will connect to (array of 4 bytes)
- URL: the domain name the client will connect to (string e.g., "arduino.cc")
- port: the port that the client will connect to (int)

#### Returns
Returns true if the connection succeeds, false if not.

#### Example

```
…

/*
  Web client

 This sketch connects to a website through an SSL connection
 using a WiFi board.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * WiFiNINA supported board

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */


#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "yourNetwork"; //  your network SSID (name)
char pass[] = "secretPassword";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = "arduino.tips";    // name address for Arduino (using DNS)

// Initialize the WiFi client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    // Make a HTTP request:
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.tips");
    client.println("Connection: close");
    client.println();
    Serial.println("Request sent");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}


…
```

### `client.status()`
```{eval-rst}
.. index:: Client::status (C++ function)
```

#### Description
Return Connection status.

#### Syntax

```
client.status()

```

#### Parameters

- None

#### Returns

- The client connection status

#### Example

```
…

void setup() {

  Serial.begin(9600);
  while (!Serial) {
        ;  }
  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
                while (true);
  }

   while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
                status = WiFi.begin(ssid, pass);
        delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  IPAddress result;
  int err = WiFi.hostByName(server, result) ;
  if (err == 1) {
        Serial.print("IP address: ");
        Serial.println(result);
  } else {
        Serial.print("Error code: ");
        Serial.println(err);
  }


  if (client.connect(result, 80)) {
        Serial.println("connected to server");
                client.println("GET /search?q=arduino HTTP/1.1");
        client.println("Host: www.google.com");
        client.println("Connection: close");
        client.println();
  }
  Serial.print("status: ");
  Serial.println(client.status());
}

…
```

### `client.write()`
```{eval-rst}
.. index:: Client::write (C++ function)
```

#### Description
Write data to all the clients connected to a server.

#### Syntax

```
client.write(data)
client.write(buffer, size);

```

#### Parameters

- data: the outgoing byte
- buffer: the outgoing message
- size: the size of the buffer

#### Returns

- The number of bytes written. It is not necessary to read this.

### `client.print()`
```{eval-rst}
.. index:: Client::print (C++ function)
```

#### Description

Print data to the server that a client is connected to. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
client.print(data)
client.print(data, BASE)

```

#### Parameters
- data: the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers:, DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns

- byte : returns the number of bytes written, though reading that number is optional

### `client.println()`
```{eval-rst}
.. index:: Client::println (C++ function)
```

#### Description

Print data, followed by a carriage return and newline, to the server a client is connected to. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
client.println()
client.println(data)
client.print(data, BASE)

```

#### Parameters

- data (optional): the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers: DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns

- byte: return the number of bytes written, though reading that number is optional

### `client.available()`
```{eval-rst}
.. index:: Client::available (C++ function)
```

#### Description
Returns the number of bytes available for reading (that is, the amount of data that has been written to the client by the server it is connected to).

available() inherits from the Stream utility class.

#### Syntax

```
client.available()

```

#### Parameters

- None

#### Returns

- The number of bytes available.

#### Example

```
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
char servername[]="google.com";  // Google

WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(servername, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
```

### `client.peek()`
```{eval-rst}
.. index:: Client::peek (C++ function)
```

#### Description

Read a byte from the file without advancing to the next one. That is, successive calls to peek() will return the same value, as will the next call to read().

This function inherited from the Stream class. See the Stream class main page for more information.

#### Syntax

```
client.peek()
```

#### Parameters

- None

#### Returns

- b: the next byte or character
- -1: if none is available

#### Example

```
…

#include <SPI.h>
#include <BetterWiFiNINA.h>

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_PASS;      // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = "www.google.com";       // name address for Google (using DNS)


WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
        ;   }

  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        while (true);
  }

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  IPAddress result;
  int err = WiFi.hostByName(server, result) ;
  if (err == 1) {
        Serial.print("IP address: ");
        Serial.println(result);
  } else {
        Serial.print("Error code: ");
        Serial.println(err);
  }

   if (client.connect(result, 80)) {
        Serial.println("connected to server");
        client.println("GET /search?q=arduino HTTP/1.1");
        client.println("Host: www.google.com");
        client.println("Connection: close");
        client.println();
  }
}

void loop() {
    int i = 0;
  while (client.available()) {
        char c = client.peek();
        Serial.print("peek: ");
        Serial.println(c);

        Serial.print("calling a second time peek, the char is the same: ");
        c = client.peek();
        Serial.println(c);

        Serial.print("calling the read retry the char and erase from the buffer: ");
        c = client.read();
        Serial.println(c);
        if (i == 2) {
        while (1);
        }
        i++;
  }
}

…
```

### `client.read()`
```{eval-rst}
.. index:: Client::read (C++ function)
```

#### Description

Reads data from the client. If no arguments are given, it will return the next character in the buffer.

#### Syntax

```
client.read()
client.read(buffer, size);
```

#### Parameters

- buffer: buffer to hold incoming packets (char*)
- len: maximum size of the buffer (int)

#### Returns
- b: the next character in the buffer (char)
- size: the size of the data
- -1: if no data is available

### `client.flush()`
```{eval-rst}
.. index:: Client::flush (C++ function)
```

#### Description

Clears the buffer once all outgoing characters have been sent.

flush() inherits from the [Stream](https://www.arduino.cc/reference/en/language/functions/communication/stream/) utility class.

#### Syntax

```
client.flush()
```

#### Parameters
- None

#### Returns
- None

### `client.stop()`
```{eval-rst}
.. index:: Client::stop (C++ function)
```

#### Description

Disconnect from the server.

#### Syntax

```
client.stop()
```

#### Parameters

- None

#### Returns

- None

### `client.remoteIP()`
```{eval-rst}
.. index:: Client::remoteIP (C++ function)
```

#### Description
Gets the IP address of the remote connection.

#### Syntax

```
client.remoteIP()

```

#### Parameters

- None

#### Returns

- The IP address of the host the client is connected to

### `client.remotePort()`
```{eval-rst}
.. index:: Client::remotePort (C++ function)
```

#### Description
Gets the port number of the remote connection.

#### Syntax

```
client.remotePort()

```

#### Parameters

- None

#### Returns
The port of the remote host that the client is connected to

