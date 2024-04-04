/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using the WiFi module.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

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
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

WiFiSocket socket;

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
  printWifiStatus();

  //Resolve server address
  IPAddress addr;
  if (auto res = WiFi.hostByName(server, addr); res != 1) {
    Serial.print("Unable to resolve: ");
    Serial.print(server);
    Serial.print(", error ");
    Serial.println(res);
    // don't continue
    while (true);
  }

  //Create socket
  socket = WiFiSocket(WiFiSocket::Type::Stream, WiFiSocket::Protocol::TCP);
  if (!socket) {
    Serial.print("Creating socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }

  //Connect to the server
  if (!socket.connect(addr, 80)) {
    Serial.print("Cannot connect to server: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }

  Serial.println("connected to server");

  Serial.println("\nStarting request to server...");

  static const char request[] =
    "GET /search?q=arduino HTTP/1.1\r\n"
    "Host: www.google.com\r\n"
    "Connection: close\r\n"
    "\r\n";

  size_t written = 0;
  while(written < sizeof(request) - 1) {
    auto sent = socket.send(request + written, sizeof(request) - 1 - written);
    if (sent < 0) {
      Serial.print("Failed to send: error ");
      Serial.println(WiFiSocket::lastError());
      // don't continue
      while (true);
    }
    written += sent;
  }
}

void loop() {
  char buf[256];

  auto read = socket.recv(buf, sizeof(buf));
  if (read < 0) {
    auto err = WiFiSocket::lastError();
    if (err == ENOTCONN) {
      Serial.println("Server disconnected");
      socket.close();
      // do nothing forevermore:
      while (true);
    }
    Serial.print("Failed to read: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }
  // if there are incoming bytes available
  // from the server, print them:
  Serial.write(buf, read);
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
