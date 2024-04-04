/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will create a new access point.
  It will then launch a new server and print out the IP address
  to the Serial Monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 13.

  If the IP address of your board is yourAddress:
    http://yourAddress/H turns the LED on
    http://yourAddress/L turns it off

 */

#include <SPI.h>
#include <BetterWiFiNINA.h>
#include <errno.h>
#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
const char ssid[] = SECRET_SSID;        // your network SSID (name)
const char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiSocket serverSocket;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode

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

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // you're connected now, so print out the status
  printWiFiStatus();

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
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

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
        digitalWrite(led, HIGH);               // GET /H turns the LED on
      }
      if (currentLine.endsWith("GET /L")) {
        digitalWrite(led, LOW);                // GET /L turns the LED off
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
      "Click <a href=\"/H\">here</a> turn the LED on<br>"
      "Click <a href=\"/L\">here</a> turn the LED off<br>"
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

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}
