/*
  Repeating WiFi Web Client

 This sketch connects to a a web server and makes a request
 using a WiFi equipped Arduino board.
 
 This code is in the public domain.
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

WiFiSocket socket;

// server address:
char server[] = "example.org";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds

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
}

void loop() {
  // if the socket is valid read incoming data from the net connection.
  // and send it out the serial port.  This is for debugging
  // purposes only:
  if (socket) {
    char buf[256];

    while( true ) {
      auto read = socket.recv(buf, sizeof(buf));
      if (read < 0) {
        auto err = WiFiSocket::lastError();
        if (err == ENOTCONN) {
          Serial.println("Server disconnected");
        } else {
          Serial.print("Failed to read: error ");
          Serial.println(WiFiSocket::lastError());
        }
        socket.close();
        break;
      } else {
        //print the bytes we read
        Serial.write(buf, read);
      }
    }
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  //Create socket (this will close existing one)
  socket = WiFiSocket(WiFiSocket::Type::Stream, WiFiSocket::Protocol::TCP);
  if (!socket) {
    Serial.print("Creating socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    return;
  }

  //Resolve server address
  IPAddress addr;
  if (auto res = WiFi.hostByName(server, addr); res != 1) {
    Serial.print("Unable to resolve: ");
    Serial.print(server);
    Serial.print(", error ");
    Serial.println(res);
    return;
  }

  //Connect to the server
  if (!socket.connect(addr, 80)) {
    Serial.print("Cannot connect to server: error ");
    Serial.println(WiFiSocket::lastError());
    socket.close();
    return;
  }

  Serial.println("connected, sending...");

  // send the HTTP GET request:
  static const char request[] =
    "GET / HTTP/1.1\r\n"
    "Host: example.org\r\n"
    "User-Agent: ArduinoWiFi/1.1\r\n"
    "Connection: close\r\n"
    "\r\n";

  size_t written = 0;
  while(written < sizeof(request) - 1) {
    auto sent = socket.send(request + written, sizeof(request) - 1 - written);
    if (sent < 0) {
      Serial.print("Failed to send: error ");
      Serial.println(WiFiSocket::lastError());
      socket.close();
      return;
    }
    written += sent;
  }

  // note the time that the connection was made:
  lastConnectionTime = millis();
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
