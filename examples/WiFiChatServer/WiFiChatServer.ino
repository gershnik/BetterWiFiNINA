/*
 Chat  Server

 A simple server that distributes any incoming messages to all
 connected clients. To use, telnet to your device's IP address and type.
 You can see the client's input in the serial monitor as well.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.


 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and Uno WiFi Rev.2)

 */

#include <SPI.h>
#include <BetterWiFiNINA.h>
#include <errno.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

struct ChatClient {
  String toWrite;     //content that needs to be written to this client
  WiFiSocket socket;  //client's socket
};

WiFiSocket serverSocket;
ChatClient clients[32] = {};
uint32_t freeClientBitmask = 0;  //bitmaks of which of the clients array is free

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

  // you're connected now, so print out the status:
  printWifiStatus();

  //Create server socket
  serverSocket = WiFiSocket(WiFiSocket::Type::Stream, WiFiSocket::Protocol::TCP);
  if (!serverSocket) {
    Serial.print("Creating server socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }
  //Bind to port 23
  if (!serverSocket.bind(23)) {
    Serial.print("Binding server socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }

  //set the server socket to non-blocking
  if (!serverSocket.setNonBlocking(true))  {
    Serial.println("Setting server socket to non-blocking failed: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }

  //And start listening
  if (!serverSocket.listen(5)) {
    Serial.print("Listen on server socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }
}


void acceptConnectionIfPossible() {

  //no clear bits => we cannot accept more connections
  if (freeClientBitmask == uint32_t(-1))
    return;

  //find first clear bit in freeClientBitmask
  //see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
  //for explantation how this works
  uint32_t firstClearBit = __builtin_clzl(~freeClientBitmask);

  //Accept incoming connection
  IPAddress addr;
  uint16_t port;
  auto acceptedSocket = serverSocket.accept(addr, port);
  if (!acceptedSocket) {
    auto err = WiFiSocket::lastError();
    if (err == EWOULDBLOCK)
      return;
    Serial.print("Accept on server socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    delay(100);
    return;
  }

  //set the session socket to non-blocking
  if (!acceptedSocket.setNonBlocking(true))  {
    Serial.print("Setting socket to non-blocking failed: error ");
    Serial.println(WiFiSocket::lastError());
    delay(100);
    return;
  }

  Serial.print("We have a new client: ");
  Serial.println(firstClearBit);

  clients[firstClearBit] = { 
    .toWrite = "Hello, client!",
    .socket = static_cast<WiFiSocket &&>(acceptedSocket)
  };
  freeClientBitmask |= (uint32_t(1) << (31 - firstClearBit));
}

void writeToIfNeeded(size_t clientIdx) {
  const uint32_t clientBit = uint32_t(1) << (31 - clientIdx);

  if ((freeClientBitmask & clientBit) == 0)
    return;
  
  auto & client = clients[clientIdx];
  if (client.toWrite.length() == 0)
    return;

  auto sent = client.socket.send(client.toWrite.c_str(), client.toWrite.length());
  if (sent < 0) {
    auto err = WiFiSocket::lastError();
    if (err == EWOULDBLOCK)
      return;

    Serial.print("writing to client ");
    Serial.print(clientIdx);
    Serial.print(" failed with error: ");
    Serial.println(err);

    client.socket.close();
    freeClientBitmask &= ~clientBit;
    return;
  }
  client.toWrite.remove(0, sent);
}

void readFromIfPossible(size_t clientIdx) {
  const uint32_t clientBit = uint32_t(1) << (31 - clientIdx);

  if ((freeClientBitmask & clientBit) == 0)
    return;

  auto & client = clients[clientIdx];

  char buffer[256];
  auto read = client.socket.recv(buffer, sizeof(buffer));
  if (read < 0) {
    auto err = WiFiSocket::lastError();
    if (err == EWOULDBLOCK)
      return;
    Serial.print("reading from client ");
    Serial.print(clientIdx);
    Serial.print(" failed with error: ");
    Serial.println(err);
    
    client.socket.close();
    freeClientBitmask &= ~clientBit;
    return;
  }

  if (read == 0)
    return;

  Serial.write(buffer, read); // print what we read to the serial monitor

  //add the message to other active clients' send buffers
  for(size_t otherClientIdx = 0; otherClientIdx < sizeof(clients)/sizeof(clients[0]); ++otherClientIdx) {
    if (otherClientIdx == clientIdx)
      continue;
    auto & otherClient = clients[otherClientIdx];
    if (otherClient.socket) {
      otherClient.toWrite.concat(buffer, read);
    }
  }
}

void loop() {

  acceptConnectionIfPossible();

  for(size_t clientIdx = 0; clientIdx < sizeof(clients)/sizeof(clients[0]); ++clientIdx) {
    writeToIfNeeded(clientIdx);
  }
  for(size_t clientIdx = 0; clientIdx < sizeof(clients)/sizeof(clients[0]); ++clientIdx) {
    readFromIfPossible(clientIdx);
  }

}


void printWifiStatus() {
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
