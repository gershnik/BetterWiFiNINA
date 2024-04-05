/*
  WiFi Web Server

 A simple web server that shows the value of the analog input pins.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * Analog inputs attached to pins A0 through A5 (optional)

 */

#include <SPI.h>
#include <BetterWiFiNINA.h>
#include <errno.h>
#include <stdio.h>


#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiSocket serverSocket;

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
  //Bind to port 80
  if (!serverSocket.bind(80)) {
    Serial.print("Binding server socket failed: error ");
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


void loop() {
  //Accept incoming connection
  IPAddress addr;
  uint16_t port;
  auto sessionSocket = serverSocket.accept(addr, port);
  if (!sessionSocket) {
    Serial.print("Accept on server socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    delay(100);
    return;
  }

  Serial.println("new client");

  //set the session socket to non-blocking
  if (!sessionSocket.setNonBlocking(true))  {
    Serial.print("Setting socket to non-blocking failed: error ");
    Serial.println(WiFiSocket::lastError());
    delay(100);
    return;
  }

  char buffer[256];
  // an HTTP request ends with a blank line
  boolean currentLineIsBlank = true;
  bool doneReading = false;

  //read until \n\r\n
  while(!doneReading) {
    auto read = sessionSocket.recv(buffer, sizeof(buffer));
    if (read < 0) {
      auto err = WiFiSocket::lastError();
      if (err == EWOULDBLOCK)
        continue;
      Serial.print("reading from socket failed with error: ");
      Serial.println(err);
      return;
    }
    Serial.write(buffer, read); // print it out to the serial monitor
    for(int i = 0; i != read; ++i) {
      char c = buffer[i];
      // if you've gotten to the end of the line (received a newline
      // character) and the line is blank, the HTTP request has ended,
      // so you can send a reply
      if (c == '\n' && currentLineIsBlank) {
        doneReading = true;
        break;
      } 
      if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
      } else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }
  }

  Serial.println("responding");

  //Our response
  static const char responsePrefix[] = 
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Connection: close\r\n"
      "Refresh: 5\r\n"
      "\r\n"
      "<!DOCTYPE HTML>\n"
      "<html><body>";
  static const char responseSuffix[] =
      "</body></html>";

  if (!writeBuffer(sessionSocket, responsePrefix, sizeof(responsePrefix) - 1)) {
    return;
  }

  // output the value of each analog input pin
  for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
    int sensorReading = analogRead(analogChannel);
    sprintf(buffer, "analog input %d is %d<br />", analogChannel, sensorReading);
    if (!writeBuffer(sessionSocket, buffer, strlen(buffer))) {
      return;
    }
  }
      
  if (!writeBuffer(sessionSocket, responseSuffix, sizeof(responseSuffix) - 1)) {
    return;
  }

  Serial.println("client disconnected");
}

bool writeBuffer(WiFiSocket & socket, const char * buffer, size_t size) {
  size_t written = 0;
  while(written != size) {
    auto sent = socket.send(buffer + written, size - written);
    if (sent < 0) {
      auto err = WiFiSocket::lastError();
      if (err == EWOULDBLOCK)
        continue;
      Serial.print("writing to socket failed with error: ");
      Serial.println(err);
      return false;
    }
    written += sent;
  }
  return true;
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
