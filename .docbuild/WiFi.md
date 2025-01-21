# WiFi

## WiFi Class

### `WiFi.begin()`
```{eval-rst}
.. index:: WiFi::begin  (C++ function)
```

#### Description
Initializes the WiFiNINA library's network settings and provides the current status.

#### Syntax

```cpp
WiFi.begin(ssid);
WiFi.begin(ssid, pass);
WiFi.begin(ssid, keyIndex, key);
```

#### Parameters

- ssid: the SSID (Service Set Identifier) is the name of the WiFi network you want to connect to.
- keyIndex: WEP encrypted networks can hold up to 4 different keys. This identifies which key you are going to use.
- key: a hexadecimal string used as a security code for WEP encrypted networks.
- pass: WPA encrypted networks use a password in the form of a string for security.

#### Returns

- WL_CONNECTED when connected to a network
- WL_IDLE_STATUS when not connected to a network, but powered on

#### Example

```cpp
#include <BetterWiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
 WiFi.begin(ssid, pass);
}

void loop () {}
```

### `WiFi.end()`
```{eval-rst}
.. index:: WiFi::end (C++ function)
```

#### Description
Turns off the WiFi module. If WiFi.begin() was used to connect to an access point, the connection will be disconnected.
If WiFi.beginAP() was used before to create an access point, the WiFi.end() will stop listening it too.

#### Syntax

```cpp
WiFi.end();

```

#### Parameters

- None

#### Returns

- Nothing

### `WiFi.beginAP()`
```{eval-rst}
.. index:: WiFi::beginAP (C++ function)
```

#### Description

Initializes the WiFiNINA library in Access Point (AP) mode. Other WiFi devices will be able to discover and connect to the created Access Point.

#### Syntax

```cpp
WiFi.beginAP(ssid);
WiFi.beginAP(ssid, channel);
WiFi.beginAP(ssid, passphrase);
WiFi.beginAP(ssid, passphrase, channel);
```

#### Parameters

- ssid: the SSID (Service Set Identifier) of the created Access Point. Must be 8 or more characters.
- passphrase: optional, the WPA password of the created Access Point. Must be 8 or more characters.
- channel: optional, channel of created Access Point (1 - 14). Defaults to channel 1;

#### Returns

- WL_AP_LISTENING when creating access point succeeds
- WL_CONNECT_FAILED when creating access point fails

#### Example

```cpp
#include <BetterWiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";


void setup() {
  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }
}

void loop() {}

```

### `WiFi.beginEnterprise()`
```{eval-rst}
.. index:: WiFi::beginEnterprise (C++ function)
```

#### Description

Initializes the WiFiNINA library's network settings for a common WPA2 Enterprise network with username and password authentication (PEAP/MSCHAPv2).

Note: this feature requires NINA firmware version 1.3.0 or later. All string parameter supplied must have a combined length of under 4000 bytes.

#### Syntax

```cpp
WiFi.beginEnterprise(ssid, username, password);
WiFi.beginEnterprise(ssid, username, password, identity);
WiFi.beginEnterprise(ssid, username, password, identity, ca);
```

#### Parameters

- ssid: the SSID (Service Set Identifier) is the name of the WiFi network you want to connect to.
- username: username part of WPA2 Enterprise (RADIUS) credentials
- password: password part of WPA2 Enterprise (RADIUS) credentials
- identity: WPA2 enterprise identity (optional)
- ca: root certificate (string) to validate against (optional)

#### Returns

- WL_CONNECTED when connected to a network
- WL_IDLE_STATUS when not connected to a network, but powered on

#### Example

```cpp
#include <BetterWiFiNINA.h>

char ssid[] = SECRET_SSID;  // your WPA2 enterprise network SSID (name)
char user[] = SECRET_USER;  // your WPA2 enterprise username
char pass[] = SECRET_PASS;  // your WPA2 enterprise password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

void setup() {
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA2 enterprise network:
    // - You can optionally provide additional identity and CA cert (string) parameters if your network requires them:
    //      WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
    status = WiFi.beginEnterprise(ssid, user, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");

}

void loop() {}
```

### `WiFi.disconnect()`
```{eval-rst}
.. index:: WiFi::disconnect (C++ function)
```

#### Description

Disconnects the WiFi from the current network.

#### Syntax

```
WiFi.disconnect()
```

#### Parameters

- None

#### Returns

- Nothing

### `WiFi.config()`
```{eval-rst}
.. index:: WiFi::config (C++ function)
```

#### Description

WiFi.config() allows you to configure a static IP address as well as change the DNS, gateway, and subnet addresses on the WiFi shield.

Unlike WiFi.begin() which automatically configures the WiFi shield to use DHCP, WiFi.config() allows you to manually set the network address of the shield.

Calling WiFi.config() before WiFi.begin() forces begin() to configure the WiFi shield with the network addresses specified in config().

You can call WiFi.config() after WiFi.begin(), but the shield will initialize with begin() in the default DHCP mode. Once the config() method is called, it will change the network address as requested.

#### Syntax

```cpp
WiFi.config(ip);
WiFi.config(ip, dns);
WiFi.config(ip, dns, gateway);
WiFi.config(ip, dns, gateway, subnet);
```

#### Parameters

- ip: the IP address of the device (array of 4 bytes)
- dns: the address for a DNS server.
- gateway: the IP address of the network gateway (array of 4 bytes). - optional: defaults to the device IP address with the last octet set to 1
- subnet: the subnet mask of the network (array of 4 bytes). optional: defaults to 255.255.255.0

#### Returns

- Nothing

#### Example

This example shows how to set the static IP address, `192.168.0.177`, of the LAN network to the WiFi:

```cpp
#include <BetterWiFiNINA.h>

// the IP address for the shield:
IPAddress ip(192, 168, 0, 177);    

char ssid[] = "yourNetwork";    // your network SSID (name)
char pass[] = "secretPassword"; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

void setup()
{ 
  WiFi.config(ip);

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void loop () {}
```

### `WiFi.setDNS()`
```{eval-rst}
.. index:: WiFi::setDNS (C++ function)
```

#### Description

WiFi.setDNS() allows you to configure the DNS (Domain Name System) server.

#### Syntax

```cpp
WiFi.setDNS(dns_server1)
WiFi.setDNS(dns_server1, dns_server2)

```

#### Parameters

- dns_server1: the IP address of the primary DNS server
- dns_server2: the IP address of the secondary DNS server

#### Returns

- Nothing

#### Example

This example shows how to set the Google DNS (8.8.8.8). You can set it as an object IPAddress.

```cpp
#include <BetterWiFiNINA.h>

// the IP address for the shield:
IPAddress dns(8, 8, 8, 8);  //Google DNS  

char ssid[] = "yourNetwork";    // your network SSID (name)
char pass[] = "secretPassword"; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

void setup()
{  
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // print your WiFi's IP address:
  WiFi.setDNS(dns);
  Serial.print("DNS configured.");
}

void loop () {}
 
```

### `WiFi.setHostname()`
```{eval-rst}
.. index:: WiFi::setHostname (C++ function)
```

#### Description
Sets the hostname of the module, the hostname is sent in WiFi.begin(...) when an IP address is requested from a DHCP server.

#### Syntax

```cpp
WiFi.setHostname(hostname)

```

#### Parameters

- hostname - new hostname to use

#### Returns

- Nothing

#### Example

```cpp
...
  WiFi.setHostname("MyArduino");

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
...
```

### `WiFi.setTimeout()`
```{eval-rst}
.. index:: WiFi::setTimeout (C++ function)
```

#### Description
Sets the connection timeout value in milliseconds for WiFi.begin(...).

#### Syntax

```cpp
WiFi.setTimeout(timeout)
```

#### Parameters

- timeout - the connection timeout value in milliseconds

#### Returns

- Nothing

#### Example

```cpp
...
 WiFi.setTimeout(120 * 1000);

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

...
```

### `WiFi.SSID()`
```{eval-rst}
.. index:: WiFi::SSID (C++ function)
```

#### Description
Gets the SSID of the current network

#### Syntax

```cpp
WiFi.SSID();
WiFi.SSID(wifiAccessPoint)
```

#### Parameters

- wifiAccessPoint: specifies from which network to get the information

#### Returns

- A string containing the SSID the WiFi is currently connected to.
- A string containing name of network requested.

#### Example

```cpp
#include <BetterWiFiNINA.h>


void setup()
{
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  // print the list of networks seen:
  Serial.print("SSID List:");
  Serial.println(numSsid);
  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") Network: ");
    Serial.println(WiFi.SSID(thisNet));
  }
}

void loop () {}

 
```

### `WiFi.BSSID()`
```{eval-rst}
.. index:: WiFi::BSSID (C++ function)
```

#### Description
Gets the MAC address of the router you are connected to or the MAC address of a network that was scanned.

#### Syntax

```cpp
WiFi.BSSID(bssid)
WiFi.BSSID(wifiAccessPoint, bssid)

```

#### Parameters

- bssid - 6 byte array
- wifiAccessPoint - specifies from which network to get the information (optional), only needed after a scan

#### Returns

A byte array containing the MAC address of the router the WiFi shield is currently connected to or the MAC address of a network that was scanned. The first array index contains the last byte of the MAC address.

#### Example

```cpp
#include <BetterWiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
  WiFi.begin(ssid, pass);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {
  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);
  }
}

void loop () {}
```

### `WiFi.RSSI()`
```{eval-rst}
.. index:: WiFi::RSSI (C++ function)
```

#### Description
Gets the signal strength of the connection to the router

#### Syntax

```cpp
WiFi.RSSI();
WiFi.RSSI(wifiAccessPoint);
```

#### Parameters

- wifiAccessPoint: specifies from which network to get the information

#### Returns

- long : the current RSSI/Received Signal Strength in dBm

#### Example

```cpp
#include <BetterWiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
  WiFi.begin(ssid, pass);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);
}

void loop () {}
```

### `WiFi.channel()`
```{eval-rst}
.. index:: WiFi::channel (C++ function)
```

#### Description
Gets the WiFi channel of a network that was scanned.

#### Syntax

```cpp
WiFi.channel(wifiAccessPoint)

```

#### Parameters

- wifiAccessPoint - specifies from which network to get the information

#### Returns

- WiFi channel of scanned network

#### Example

```cpp
...
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks: ");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet + 1);
    Serial.print(") ");
    Serial.print("Signal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tChannel: ");
    Serial.print(WiFi.channel(thisNet));
    byte bssid[6];
    Serial.print("\t\tBSSID: ");
    printMacAddress(WiFi.BSSID(thisNet, bssid));
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
    Serial.print("\t\tSSID: ");
    Serial.println(WiFi.SSID(thisNet));
    Serial.flush();
  }
  Serial.println();

...
```

### `WiFi.encryptionType()`
```{eval-rst}
.. index:: WiFi::encryptionType (C++ function)
```

#### Description
Gets the encryption type of the current network

#### Syntax

```cpp
WiFi.encryptionType();
WiFi.encryptionType(wifiAccessPoint);
```

#### Parameters

- wifiAccessPoint: specifies which network to get information from

#### Returns

byte : value represents the type of encryption

- TKIP (WPA) = 2
- WEP = 5
- CCMP (WPA) = 4
- NONE = 7
- AUTO = 8

#### Example

```cpp
#include <SPI.h>
#include <BetterWiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
  WiFi.begin(ssid, pass);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
}

void loop () {}
```

### `WiFi.scanNetworks()`
```{eval-rst}
.. index:: WiFi::scanNetworks (C++ function)
```

#### Description
Scans for available WiFi networks and returns the discovered number

#### Syntax

```cpp
WiFi.scanNetworks()

```

#### Parameters

- None

#### Returns

- byte : number of discovered networks

#### Example

See [ScanNetworks](https://github.com/gershnik/BetterWiFiNINA/tree/mainline/examples/ScanNetworks/ScanNetworks.ino) and [ScanNetworksAdvanced](https://github.com/gershnik/BetterWiFiNINA/tree/mainline/examples/ScanNetworksAdvanced/ScanNetworksAdvanced.ino) examples.

```cpp

#include <SPI.h>
#include <BetterWiFiNINA.h>

void setup() {
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  printMacAddress(mac);
}

void loop() {
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
  delay(10000);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
    case ENC_TYPE_UNKNOWN:
    default:
      Serial.println("Unknown");
      break;
  }
}


void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
```

### `WiFi.ping()`
```{eval-rst}
.. index:: WiFi::ping (C++ function)
```

#### Description

- Ping a remote device on the network.

#### Syntax

```cpp
WiFi.ping(ip);
WiFi.ping(ip, ttl);
WiFi.ping(host);
WiFi.ping(host, ttl);
```

#### Parameters

- ip: the IP address to ping (array of 4 bytes)
- host: the host to ping (string)
- ttl: Time of live (optional, defaults to 128). Maximum number of routers the request can be forwarded to.

#### Returns

- WL_PING_SUCCESS when the ping was successful
- WL_PING_DEST_UNREACHABLE when the destination (IP or host is unreachable)
- WL_PING_TIMEOUT when the ping times out
- WL_PING_UNKNOWN_HOST when the host cannot be resolved via DNS
- WL_PING_ERROR when an error occurs

#### Example

See [WiFiPing](https://github.com/gershnik/BetterWiFiNINA/tree/mainline/examples/WiFiPing/WiFiPing.ino) example

### `WiFi.status()`
```{eval-rst}
.. index:: WiFi::status (C++ function)
```

#### Description
Return the connection status.

#### Syntax

```cpp
WiFi.status()

```

#### Parameters

- None

#### Returns

- WL_CONNECTED: assigned when connected to a WiFi network;
- WL_AP_CONNECTED : assigned when a device is connected in Access Point mode;
- WL_AP_LISTENING : assigned when the listening for connections in Access Point mode;
- WL_NO_SHIELD: assigned when no WiFi shield is present;
- WL_NO_MODULE: assigned when the communication with an integrated WiFi module fails;
- WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
- WL_NO_SSID_AVAIL: assigned when no SSID are available;
- WL_SCAN_COMPLETED: assigned when the scan networks is completed;
- WL_CONNECT_FAILED: assigned when the connection fails for all the attempts;
- WL_CONNECTION_LOST: assigned when the connection is lost;
- WL_DISCONNECTED: assigned when disconnected from a network;

#### Example

```cpp
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "yourNetwork";                     // your network SSID (name)
char key[] = "D0D0DEADF00DABBADEAFBEADED";       // your network key
int keyIndex = 0;                                // your network key Index number
int status = WL_IDLE_STATUS;                     // the WiFi radio's status

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WEP network, SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, keyIndex, key);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // once you are connected :
  Serial.print("You're connected to the network");
}

void loop() {
  // check the network status connection once every 10 seconds:
  delay(10000);
  Serial.println(WiFi.status());
}

 
```

### `WiFi.macAddress()`
```{eval-rst}
.. index:: WiFi::macAddress (C++ function)
```

#### Description
Gets the MAC Address of your WiFi NINA module

#### Syntax

```cpp
WiFi.macAddress(mac)

```

#### Parameters

- mac: a 6 byte array to hold the MAC address

#### Returns

- byte array : 6 bytes representing the MAC address of your module

#### Example

```cpp
#include <SPI.h>
#include <BetterWiFiNINA.h>

char ssid[] = "yourNetwork";     // the name of your network
int status = WL_IDLE_STATUS;     // the WiFi radio's status

byte mac[6];                     // the MAC address of your WiFi Module


void setup()
{
  ...
  status = WiFi.begin(ssid);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void loop () {}
```

### `WiFi.firmwareVersion()`
```{eval-rst}
.. index:: WiFi::firmwareVersion (C++ function)
```

#### Description
Returns the firmware version running on the module as a string.

#### Syntax

```cpp
WiFi.firmwareVersion()

```

#### Parameters

- None

#### Returns

- The firmware version running on the module as a string

#### Example

```cpp
...
String fv = WiFi.firmwareVersion();
if (fv < "1.0.0") {
  Serial.println("Please upgrade the firmware");
}
...
```

### `WiFi.lowPowerMode()`
```{eval-rst}
.. index:: WiFi::lowPowerMode (C++ function)
```

#### Description

Enable low power mode. This is an automatically managed mode where the WiFi NINA Module reduces its power drain bringing the overall power consumption to 30 mA. Any incoming data is received and the device sends out regularly the beacon signal each 100 ms to keep the AP connection alive.

#### Syntax

```cpp
WiFi.lowPowerMode()
```

#### Returns

- None

### `WiFi.noLowPowerMode()`
```{eval-rst}
.. index:: WiFi::noLowPowerMode (C++ function)
```

#### Description

Disables the power saving modes enabled with lowPowerMode(). This is the default status of Power Mode.

#### Syntax

```cpp
WiFi.noLowPowerMode()
```

#### Returns

- None

### `WiFi.reasonCode()`
```{eval-rst}
.. index:: WiFi::reasonCode (C++ function)
```

#### Description

Return The deauthentication reason code.

#### Syntax

```cpp
WiFi.reasonCode()

```

#### Parameters

- None

#### Returns

- The deauthentication reason code

#### Example

```cpp
...

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    if (status != WL_CONNECTED) {

      Serial.print("Reason code: ");
      Serial.println(WiFi.reasonCode());

    }
    // wait 10 seconds for connection:
    delay(10000);
  }

...
```

### `WiFi.hostByName()`
```{eval-rst}
.. index:: WiFi::hostByName (C++ function)
```

#### Description
Resolve the given hostname to an IP address

#### Syntax

```cpp
WiFi.hostByName(hostname, result)

```

#### Parameters

- hostname: Name to be resolved
- result: IPAddress structure to store the returned IP address

#### Returns

- 1 if hostname was successfully converted to an IP address, else the error code

#### Example

```cpp
...

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
  if(err == 1){
        Serial.print("IP address: ");
        Serial.println(result);
  } else {
        Serial.print("Error code: ");
        Serial.println(err);
  }

...
```

### `WiFi.localIP()`
```{eval-rst}
.. index:: WiFi::localIP (C++ function)
```

#### Description

Gets the WiFi's IP address

#### Syntax

```cpp
WiFi.localIP()

```

#### Parameters
- None

#### Returns
- the IP address of the board

#### Example

```cpp
...

WiFi.begin(ssid);

if ( status != WL_CONNECTED) {
  Serial.println("Couldn't get a WiFi connection");
  while(true);
}
//print the local IP address
ip = WiFi.localIP();
Serial.println(ip);

```

### `WiFi.subnetMask()`
```{eval-rst}
.. index:: WiFi::subnetMask (C++ function)
```

#### Description
Gets the WiFi's subnet mask

#### Syntax

```cpp
WiFi.subnetMask()

```

#### Parameters

- None

#### Returns

- the subnet mask of the board

#### Example

```cpp
...
WiFi.begin(ssid, pass);

if ( status != WL_CONNECTED) {
  Serial.println("Couldn't get a WiFi connection");
  while(true);
}
// print your subnet mask:
IPAddress subnet = WiFi.subnetMask();
Serial.print("NETMASK: ");
Serial.println();
 
```

### `WiFi.gatewayIP()`
```{eval-rst}
.. index:: WiFi::gatewayIP (C++ function)
```

#### Description
Gets the WiFi's gateway IP address.

#### Syntax

```cpp
WiFi.gatewayIP()

```

#### Parameters

- None

#### Returns

- An array containing the board's gateway IP address

#### Example

```cpp
...

WiFi.begin(ssid, pass);

if ( status != WL_CONNECTED) {
  Serial.println("Couldn't get a WiFi connection");
  while(true);
}
// print your gateway address:
IPAddress gateway = WiFi.gatewayIP();
Serial.print("GATEWAY: ");
Serial.println(gateway);

```

### `WiFi.dnsIP()`
```{eval-rst}
.. index:: WiFi::dnsIP (C++ function)
```

#### Description

Returns the DNS server IP address for the device.


#### Syntax

```
WiFi.dnsIP()
WiFi.dnsIP(n)

```

#### Parameters
optional parameter n for the number of the DNS server to get the second DNS serverv

#### Returns
- the DNS server IP address for the device (IPAddress).

#### Example

```cpp

IPAddress emptyIP;

int status = WiFi.begin(ssid, pass);
if ( status != WL_CONNECTED) {
  Serial.println("Couldn't get a WiFi connection");
  while(true);
}

Serial.print("DHCP assigned DNS server: ");
IPAddress dns1 = WiFi.dnsIP();
if (dns1 == emptyIP) {
  Serial.println("not set");
} else {
  dns1.printTo(Serial);
  Serial.println();
  IPAddress dns2 = WiFi.dnsIP(1);
  if (dns2 != emptyIP) {
    Serial.print("DNS server2: ");
    dns2.printTo(Serial);
    Serial.println();
  }
}
```

### `WiFi.getTime()`
```{eval-rst}
.. index:: WiFi::getTime (C++ function)
```

#### Description
Get the time in seconds since January 1st, 1970. The time is retrieved from the WiFi module which periodically fetches the NTP time from an NTP server.

#### Syntax

```cpp
WiFi.getTime();
```

#### Parameters

- None

#### Returns

- Returns the time in seconds since January 1st, 1970 on success. 0 on failure.

