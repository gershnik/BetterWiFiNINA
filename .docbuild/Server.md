# Deprecated: Server

The classes described here are deprecated and kept for backward compatibility with WiFiNINA only.
You should use [WiFiSocket](Socket.rst) class instead.

## Server Class

### `Server()`

#### Description

Server is the base class for all WiFi server based calls. It is not called directly, but invoked whenever you use a function that relies on it.

### `WiFiServer()`

#### Description

Creates a server that listens for incoming connections on the specified port.

#### Syntax

```
WiFiServer server(port);
```

#### Parameters
- port: the port to listen on (int)

#### Returns
- None

#### Example

```
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("Connected to WiFi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);

  }
}


void loop() {

}
 
```

### `server.begin()`

#### Description
Tells the server to begin listening for incoming connections.

#### Syntax

```
server.begin()

```

#### Parameters

- None

#### Returns

- None

#### Example

```
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "lamaison";          //  your network SSID (name)
char pass[] = "tenantaccess247";   // your network password
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("Connected to WiFi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);

  }
}


void loop() {

}
 
```

### `server.status()`

#### Description
Return Connection status.

#### Syntax

```
server.status()
```

#### Parameters

- None

#### Returns

- The server connection status

#### Example

```
…

Serial.print("connection status: ");
Serial.println(client.status());

…
```

### `server.write()`

#### Description
Write data to all the clients connected to a server.

#### Syntax

```
server.write(data)
server.write(buffer, size);

```

#### Parameters

- data: the outgoing byte
- buffer: the outgoing message
- size: the size of the buffer

#### Returns

- The number of bytes written. It is not necessary to read this.

#### Example

```
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "yourNetwork";
char pass[] = "yourPassword";
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
  }
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client == true) {
       // read bytes from the incoming client and write them back
    // to any clients connected to the server:
    server.write(client.read());
  }
}
```

### `server.print()`

#### Description

Print data to all the clients connected to a server. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
server.print(data)
server.print(data, BASE)

```

#### Parameters
- data: the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers: BIN for binary (base 2), DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns
- byte
- print() will return the number of bytes written, though reading that number is optional

### `server.println()`

#### Description
Prints data, followed by a newline, to all the clients connected to a server. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
server.println()
server.println(data)
server.println(data, BASE)

```

#### Parameters

- data (optional): the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers: DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns
- byte
- println() will return the number of bytes written, though reading that number is optional


### `server.available()`

#### Description
Gets a client that is connected to the server and has data available for reading. The connection persists when the returned client object goes out of scope; you can close it by calling client.stop().

available() inherits from the Stream utility class.

#### Syntax

```
server.available()
```

#### Parameters

- None

#### Returns

- a Client object; if no Client has data available for reading, this object will evaluate to false in an if-statement

#### Example

```
#include <SPI.h>
#include <NINA.h>

char ssid[] = "Network";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("Connected to WiFi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);

  }
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {

    if (client.connected()) {
      Serial.println("Connected to client");
    }

    // close the connection:
    client.stop();
  }
}
 
```

### `server.peek()`

#### Description

Read a byte from the file without advancing to the next one. That is, successive calls to peek() will return the same value, as will the next call to read().

This function inherited from the Stream class. See the Stream class main page for more information.

#### Syntax

```
server.peek()

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

char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_PASS;      // your network password

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {

  Serial.begin(9600);
  while (!Serial) {

  }


  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
  }

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);

        status = WiFi.begin(ssid, pass);


        delay(10000);
  }
  server.begin();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        int i = 0;
        while (client.connected()) {
        if (client.available()) {
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

        if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
        } else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
        }
        }
        }
        // give the web browser time to receive the data
        delay(1);

        // close the connection:
        client.stop();
        Serial.println("client disconnected");
  }
}

…
```
