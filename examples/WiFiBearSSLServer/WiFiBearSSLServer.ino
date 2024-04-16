/*
  WiFi Bear SSL Server

 A simple SSL web server.

 This example requires ArduinoBearSSL library

 */

//Define LARGE_MEMORY_DEVICE to 1 if your device has at least 128kB RAM

//If your board is very memory constrained you can save more memory by 
//dropping PEM parsing below and pre-parse the certificates using Bear SSL
//`brssl` command line tool. See Bear SSL documentation for more details.

#ifdef ARDUINO_NANO_RP2040_CONNECT
  #define LARGE_MEMORY_DEVICE 1
#endif


//BetterWiFiNINA library
//Obtain at https://github.com/gershnik/BetterWiFiNINA
#include <BetterWiFiNINA.h>

//ArduinoBearSSL library
//Obtain via Library Manager or https://github.com/arduino-libraries/ArduinoBearSSL
#include <ArduinoBearSSL.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
#include "arduino_secrets.h"

#include <errno.h>


br_x509_certificate g_bearCertificate;
br_skey_decoder_context g_privateKeyContext;

//Server SSL context
br_ssl_server_context g_sslContext;
//Server socket
WiFiSocket g_sslServerSocket;

//SSL session cache
br_ssl_session_cache_lru g_sessionCache;

#if LARGE_MEMORY_DEVICE

uint8_t g_inputBuffer[BR_SSL_BUFSIZE_INPUT];
uint8_t g_outputBuffer[BR_SSL_BUFSIZE_OUTPUT];
uint8_t g_sessionCacheStorage[5000];

#else 

uint8_t g_ioBuffer[BR_SSL_BUFSIZE_INPUT];

#endif


static inline void __attribute__((noreturn)) hang() {
  while(true);
}

//If your board has a source of entropy define 
//DEVICE_HAS_ENTROPY to 1 and fillRandom(buffer, size)
//function for it
#ifdef ARDUINO_NANO_RP2040_CONNECT

#define DEVICE_HAS_ENTROPY 1

//On RP2040 we can use ring oscillator as random sources
#include <hardware/structs/rosc.h>

//rosc_hw is mis-declared
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
//From: https://www.i-programmer.info/programming/148-hardware/17030-master-the-pico-wifi-random-numbers.html
uint8_t getRandomByte() {
  uint32_t random = 0;
  uint32_t bit = 0;
  for (int k = 0; k < 8; k++) {
    while (true) {
      bit = rosc_hw->randombit;
      sleep_us(10);
      if (bit != rosc_hw->randombit)
        break;
    }  
    random = (random << 1) | bit; 
    sleep_us(10); 
  }
  return uint8_t(random);
}
#pragma GCC diagnostic pop

inline void fillRandom(uint8_t * buf, size_t size) {
  for(size_t i = 0; i != size; ++i)
      buf[i] = getRandomByte();
}

#endif


template<class T>
bool parsePem(const char * pem, T dest) {
  br_pem_decoder_context pemDecoder={};
  br_pem_decoder_init(&pemDecoder);
  
  size_t size = strlen(pem);
  size_t consumed = 0;
  while(consumed != size) {
    size_t pushed = br_pem_decoder_push(&pemDecoder, pem + consumed, size - consumed);
    consumed += pushed;
    int event = br_pem_decoder_event(&pemDecoder);
    if (event == BR_PEM_BEGIN_OBJ) {
      br_pem_decoder_setdest(&pemDecoder, [](void * dest_ctx, const void * src, size_t len){
        auto sink = static_cast<T *>(dest_ctx);
        (*sink)(src, len);
      }, &dest);
    }
    if (event == BR_PEM_END_OBJ)
      return true;
    if (event == BR_PEM_ERROR) {
      return false;
    }
  }
  return false;
}

static bool loadCertificate() {
  size_t certSize = 0;
  if (!parsePem(g_certificate, [&](const void *, size_t size) {
    certSize += size;
  })) {
    Serial.print("failed to parse certificate PEM\n"); 
    return false;
  }
  g_bearCertificate.data = (uint8_t*)malloc(certSize);
  g_bearCertificate.data_len = certSize;
  uint8_t * appendPoint = g_bearCertificate.data;
  parsePem(g_certificate, [&](const void * data, size_t size) {
    memcpy(appendPoint, data, size);
    appendPoint += size;
  });
  return true;
}

static bool loadPrivateKey() {
  br_skey_decoder_init(&g_privateKeyContext);
  if (!parsePem(g_privateKey, [](const void * data, size_t size) {
    br_skey_decoder_push(&g_privateKeyContext, data, size);
  })) {
    Serial.print("failed to parse private key PEM\n");
    return false;
  }
  auto err = br_skey_decoder_last_error(&g_privateKeyContext);
  if (err) {
    Serial.print("failed to parse private key: ");
    Serial.println(err);
    return false;
  }
  return true;
}

void setup() {
  Serial.print("Starting\n");
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.print("Communication with WiFi module failed!\n");
    hang();
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.print("Please upgrade the firmware\n");
  }

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(g_ssid);
  while (true) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    int status = WiFi.begin(g_ssid, g_pass);
    if (status == WL_CONNECTED)
      break;
    delay(1000);
    WiFi.disconnect();
  }
  Serial.print("Connected\n");

  //Load certificate chain and private key
  if (!loadCertificate()) {
    hang();
  }
  if (!loadPrivateKey()) {
    hang();
  }

  //Initialize SSL context
  int keyType = br_skey_decoder_key_type(&g_privateKeyContext);
  if (keyType == BR_KEYTYPE_RSA) {
    br_ssl_server_init_full_rsa(&g_sslContext, &g_bearCertificate, 1, br_skey_decoder_get_rsa(&g_privateKeyContext));
  } else if (keyType == BR_KEYTYPE_EC) {
    br_ssl_server_init_full_ec(&g_sslContext, &g_bearCertificate, 1, BR_KEYTYPE_EC, br_skey_decoder_get_ec(&g_privateKeyContext));
  } else {
    Serial.print("unsupported private key type: ");
    Serial.println(keyType);
    hang();
  }

  #if LARGE_MEMORY_DEVICE
    //Initialize SSL session cache
    br_ssl_session_cache_lru_init(&g_sessionCache, g_sessionCacheStorage, sizeof(g_sessionCacheStorage));
    br_ssl_server_set_cache(&g_sslContext, &g_sessionCache.vtable);
  #endif
  
  //Inject entropy
  //If we have hardware entropy - use that
  //Otherwise, "entropy" must here be considered to mean "that which the attacker cannot predict". 
  //If the platform does not have a suitable source of randomness, then you can make do with the combination of a 
  //large enough secret value (possibly a copy of an asymmetric private key that you also store on the system) 
  //AND a non-repeating value (e.g. current time, provided that the local clock cannot be reset or altered by the attacker).

  #ifdef DEVICE_HAS_ENTROPY
    uint8_t entropy[32];
    fillRandom(entropy, sizeof(entropy));
    br_ssl_engine_inject_entropy(&g_sslContext.eng, entropy, sizeof(entropy));
  
  #else
    br_ssl_engine_inject_entropy(&g_sslContext.eng, g_privateKey, sizeof(g_privateKey));
    auto timing = millis();
    br_ssl_engine_inject_entropy(&g_sslContext.eng, &timing, sizeof(timing));

  #endif

  #if LARGE_MEMORY_DEVICE
    br_ssl_engine_set_buffers_bidi(&g_sslContext.eng,
                                   g_inputBuffer, sizeof(g_inputBuffer),
                                   g_outputBuffer, sizeof(g_outputBuffer));
  #else

    br_ssl_engine_set_buffer(&g_sslContext.eng, g_ioBuffer, sizeof(g_ioBuffer), 1);

  #endif


  //Create a socket
  g_sslServerSocket = WiFiSocket(WiFiSocket::Type::Stream, WiFiSocket::Protocol::TCP);
  if (!g_sslServerSocket) {
    Serial.print("Creating server socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    hang();
  }
  //Bind to port 443
  if (!g_sslServerSocket.bind(443)) {
    Serial.print("Binding server socket failed: error ");
    Serial.println(WiFiSocket::lastError()); 
    hang();
  }
  //And start listening
  if (!g_sslServerSocket.listen(5)) {
    Serial.print("Listen on server socket failed: error ");
    Serial.println(WiFiSocket::lastError()); 
    hang();
  }

  Serial.print("setup done\n");
}

void loop() {
  br_ssl_server_reset(&g_sslContext);
  IPAddress addr;
  uint16_t port;
  auto sessionSocket = g_sslServerSocket.accept(addr, port);
  if (!sessionSocket) {
    Serial.print("Accept on server socket failed: error ");
    Serial.println(WiFiSocket::lastError());
    delay(100);
    return;
  }

  //set the session socket to non-blocking
  if (!sessionSocket.setNonBlocking(true))  {
    Serial.print("Setting socket to non-blocking failed: error ");
    Serial.println(WiFiSocket::lastError());
    delay(100);
    return;
  }

  WiFiBearSSLSocket sslSessionSocket(static_cast<WiFiSocket &&>(sessionSocket), &g_sslContext.eng);

  //perform handshake
  while (true) {
    if (sslSessionSocket.handshake())
      break;
    auto err = WiFiBearSSLSocket::lastError();
    if (err != EWOULDBLOCK) {
      Serial.print("TLS handshake error: ");
      Serial.print(err);
      delay(100);
      return;
    }
  }

  uint8_t buffer[256];
  bool currentLineIsBlank = true;
  bool doneReading = false;

  //read until \n\r\n
  while(!doneReading) {
    auto read = sslSessionSocket.recv(buffer, sizeof(buffer));
    if (read < 0) {
      auto err = WiFiBearSSLSocket::lastError();
      if (err == EWOULDBLOCK)
        continue;
      Serial.print("Read error: ");
      Serial.println(err);
      delay(100);
      return;
    }
    for(int i = 0; i != read; ++i) {
      auto c = buffer[i];
      if (c == '\n' && currentLineIsBlank) {
        doneReading = true;
        break;
      }
      if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }

  //write response
  static const char response[] = 
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Connection: close\r\n"
      "\r\n"
      "<!DOCTYPE HTML>\n"
      "<html><body>Hello World!</html>";

  size_t written = 0;
  while(written != sizeof(response) - 1) {
    auto sent = sslSessionSocket.send((const uint8_t *)response + written, 
                                      sizeof(response) - 1 - written);
    if (sent < 0) {
      auto err = WiFiBearSSLSocket::lastError();
      if (err == EWOULDBLOCK)
        continue;
      Serial.print("Write error: ");
      Serial.println(err);
      delay(100);
      return;
    }
    written += sent;
  }

  while(!sslSessionSocket.finish()) {
    auto err = WiFiBearSSLSocket::lastError();
    if (err == EWOULDBLOCK)
      continue;
    Serial.print("Finish failed, error: ");
    Serial.println(err);
    delay(100);
    return;
  }
}

