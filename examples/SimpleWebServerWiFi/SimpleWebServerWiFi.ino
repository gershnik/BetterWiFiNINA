/*
  WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi module (once connected)
 to the Serial Monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 9.

 If the IP address of your board is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and Uno WiFi Rev.2)
 * LED attached to pin 9
 
 */


#include <SPI.h>
#include <BetterWiFiNINA.h>
#include <errno.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiSocket serverSocket;

void setup() {
  Serial.begin(9600);      // initialize serial communication
  pinMode(9, OUTPUT);      // set the LED pin mode

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
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  printWifiStatus();                        // you're connected now, so print out the status

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

  uint8_t buffer[256];       // a buffer to read to
  String currentLine = "";   // a String to hold each request line from the client
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
      delay(100);
      return;
    }
    Serial.write(buffer, read); // print it out to the serial monitor
    for(int i = 0; i != read; ++i) {
      char c = buffer[i];
      if (c == '\n') {                    // if the byte is a newline character
        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
          doneReading = true;
          break;
        } else {      // if you got a newline, then clear currentLine:
          currentLine = "";
        }
      } else if (c != '\r') {    // if you got anything else but a carriage return character,
        currentLine += c;      // add it to the end of the currentLine
      }

      // Check to see if the client request was "GET /H" or "GET /L":
      if (currentLine.endsWith("GET /H")) {
        digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
      }
      if (currentLine.endsWith("GET /L")) {
        digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
      }
    }
  }

  //Our response
  static const char response[] = 
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Connection: close\r\n"
      "\r\n"
      "<!DOCTYPE HTML>\n"
      "<html><body>"
      "Click <a href=\"/H\">here</a> turn the LED on pin 9 on<br>"
      "Click <a href=\"/L\">here</a> turn the LED on pin 9 off<br>"
      "</body></html>";

  size_t written = 0;
  while(written != sizeof(response)) {
    auto sent = sessionSocket.send(response + written, sizeof(response) - written);
    if (sent < 0) {
      auto err = WiFiSocket::lastError();
      if (err == EWOULDBLOCK)
        continue;
      Serial.print("writing to socket failed with error: ");
      Serial.println(err);
      delay(100);
      return;
    }
    written += sent;
  }

  Serial.println("response sent");
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
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
