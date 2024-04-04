/*
  WiFi UDP Send and Receive String

 This sketch waits for a UDP packet on localPort using the WiFi module.
 When a packet is received an Acknowledge packet is sent to the client on port remotePort

 */


#include <SPI.h>
#include <BetterWiFiNINA.h>

int status = WL_IDLE_STATUS;
#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[256]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiSocket udpSocket;

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

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  
  //Create UDP socket
  udpSocket = WiFiSocket(WiFiSocket::Type::DGram, WiFiSocket::Protocol::UDP);
  if (!udpSocket) {
    Serial.print("Creating UDP socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }
  //Bind to localPort
  if (!udpSocket.bind(localPort)) {
    Serial.print("Binding UDP socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    // don't continue
    while (true);
  }
}

void loop() {

  // read the packet into packetBuffer
  IPAddress remoteIp;
  uint16_t remotePort;
  auto packetSize = udpSocket.recvFrom(packetBuffer, sizeof(packetBuffer) - 1, remoteIp, remotePort);
  if (packetSize > 0) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(remotePort);
    packetBuffer[packetSize] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    udpSocket.sendTo(ReplyBuffer, sizeof(ReplyBuffer) - 1, remoteIp, remotePort);
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
