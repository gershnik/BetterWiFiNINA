/*
  WiFi Mbed TLS Server

 A simple SSL web server.

 This example requires:
 - Mbed OS based board
 - At least 64kB of RAM
 - Optional: MbedNanoTLS library (https://github.com/gershnik/MbedNanoTLS)

 */


//BetterWiFiNINA library
//Obtain at https://github.com/gershnik/BetterWiFiNINA
#include <BetterWiFiNINA.h>

//For better performance uncomment the line below to use MbedNanoTLS library
//#define USE_MBED_NANO_TLS 1

#if USE_MBED_NANO_TLS
  #include <MbedNanoTLS.h>
#else
  #include <mbedtls/platform.h>
  #include <mbedtls/entropy.h>
  #include <mbedtls/ssl.h>
  #include <mbedtls/ctr_drbg.h>
  #include <mbedtls/x509.h>
  #include <mbedtls/ssl.h>
  #include <mbedtls/ssl_cache.h>
  #include <mbedtls/net.h>
  #include <mbedtls/debug.h>
#endif


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
#include "arduino_secrets.h"


//Our NVRAM :)
uint8_t g_nvRandomSeed[MBEDTLS_ENTROPY_BLOCK_SIZE];
//Mbed TLS entropy and random number generator objects
mbedtls_entropy_context g_entropy;
mbedtls_ctr_drbg_context g_ctr_drbg;
//SSL config, server certificate and private key
mbedtls_ssl_config g_ssl_conf;
mbedtls_x509_crt g_srvcert;
mbedtls_pk_context g_pkey;
//Server socket
WiFiSocket g_sslServerSocket;


//Mbed TLS callbacks to access "NVRAM"
extern "C" {

    int mbedtls_platform_std_nv_seed_read(uint8_t * buf, size_t buf_len) {
        size_t toRead = std::min(buf_len, sizeof(g_nvRandomSeed));
        memcpy(buf, g_nvRandomSeed, toRead);
        return toRead;
	}

	int mbedtls_platform_std_nv_seed_write(uint8_t * buf, size_t buf_len) {
		size_t toWrite = std::min(buf_len, sizeof(g_nvRandomSeed));
        memcpy(g_nvRandomSeed, buf, toWrite);
        return toWrite;
	}
}

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

  //Inject entropy
  //If we have hardware entropy - use that
  //Otherwise, "entropy" must here be considered to mean "that which the attacker cannot predict". 
  //If the platform does not have a suitable source of randomness, then you can make do with the combination of a 
  //large enough secret value (possibly a copy of an asymmetric private key that you also store on the system) 
  //AND a non-repeating value (e.g. current time, provided that the local clock cannot be reset or altered by the attacker).

  #ifdef DEVICE_HAS_ENTROPY
    fillRandom(g_nvRandomSeed, sizeof(g_nvRandomSeed));
    
  #else

    {
      size_t idx = 0;
      for(auto b: g_privateKey) {
        g_nvRandomSeed[idx] ^= b;
        idx = (idx + 1) % sizeof(g_nvRandomSeed);
      }
      auto timing = millis();
      for(size_t i = 0; i < sizeof(timing); ++i) {
        g_nvRandomSeed[idx] ^= ((uint8_t *)&timing)[i];
        idx = (idx + 1) % sizeof(g_nvRandomSeed);
      }
    }
  #endif

  int res;
  #if MBEDTLS_VERSION_MAJOR >= 3
    //Custom MbedTLS requires to set seed access functions manually
    res = mbedtls_platform_set_nv_seed(mbedtls_platform_std_nv_seed_read, mbedtls_platform_std_nv_seed_write);
    if (res) {
      Serial.print("mbedtls_platform_set_nv_seed failed, error ");
      Serial.println(res);
      hang();
    }
  #endif

  mbedtls_entropy_init(&g_entropy);
  mbedtls_ctr_drbg_init(&g_ctr_drbg);
  res = mbedtls_ctr_drbg_seed(&g_ctr_drbg, mbedtls_entropy_func, &g_entropy, nullptr, 0);
  if (res != 0) {
    Serial.print("mbedtls_ctr_drbg_seed failed, error ");
    Serial.println(res);
    hang();
  }

  //load the certificate
  mbedtls_x509_crt_init(&g_srvcert);
  res = mbedtls_x509_crt_parse(&g_srvcert, (const uint8_t *)g_certificate, sizeof(g_certificate));
  if (res) {
    Serial.print("mbedtls_x509_crt_parse failed, error ");
    Serial.println(res);
    hang();
  }

  //load the private key
  mbedtls_pk_init(&g_pkey);
  res = mbedtls_pk_parse_key(&g_pkey, (const uint8_t *)g_privateKey, sizeof(g_privateKey), 
                             nullptr, 0
  #if MBEDTLS_VERSION_MAJOR >= 3
                             , nullptr, nullptr
  #endif
                             );
  if (res) {
    Serial.print("mbedtls_pk_parse_key failed, error ");
    Serial.println(res);
    hang();
  }


  //set up SSL config params
  mbedtls_ssl_config_init(&g_ssl_conf);
  res = mbedtls_ssl_config_defaults(&g_ssl_conf,
                                        MBEDTLS_SSL_IS_SERVER,
                                        MBEDTLS_SSL_TRANSPORT_STREAM,
                                        MBEDTLS_SSL_PRESET_DEFAULT);
  if (res) {
    Serial.print("mbedtls_ssl_config_defaults failed, error ");
    Serial.println(res);
    hang();
  }

  //connect our RNG to SSL config
  mbedtls_ssl_conf_rng(&g_ssl_conf, mbedtls_ctr_drbg_random, &g_ctr_drbg);

  //load certificate data into SSL config
  mbedtls_ssl_conf_ca_chain(&g_ssl_conf, g_srvcert.next, nullptr);
  res = mbedtls_ssl_conf_own_cert(&g_ssl_conf, &g_srvcert, &g_pkey);
  if (res) {
    Serial.print("mbedtls_ssl_conf_own_cert failed, error ");
    Serial.println(res);
    hang();
  }


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

//a simple RAII wrapper over mbedtls_ssl_context
struct SSLSessionContext : mbedtls_ssl_context {
  SSLSessionContext() { mbedtls_ssl_init(this); }
  ~SSLSessionContext() { mbedtls_ssl_free(this); }
  SSLSessionContext(const SSLSessionContext &) = delete;
  SSLSessionContext & operator=(const SSLSessionContext &) = delete;
};

void loop() {
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

  //declare and initialize SSL session context
  //with the SSL config we set up before
  SSLSessionContext sslSessionContext;
  int res = mbedtls_ssl_setup(&sslSessionContext, &g_ssl_conf);
  if (res) {
    Serial.print("mbedtls_ssl_setup failed, error ");
    Serial.println(res);
    delay(100);
    return;
  }

  //Initialize SSL socket
  WiFiMbedTLSSocket sslSessionSocket{static_cast<WiFiSocket &&>(sessionSocket), &sslSessionContext};

  //perform handshake
  while (true) {
    if (sslSessionSocket.handshake())
      break;
    auto err = WiFiMbedTLSSocket::lastError();
    if (err != MBEDTLS_ERR_SSL_WANT_READ && err != MBEDTLS_ERR_SSL_WANT_WRITE) {
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
      auto err = WiFiMbedTLSSocket::lastError();
      if (err == MBEDTLS_ERR_SSL_WANT_READ)
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
      auto err = WiFiMbedTLSSocket::lastError();
      if (err == MBEDTLS_ERR_SSL_WANT_WRITE)
          continue;
      Serial.print("Write error: ");
      Serial.println(err);
      delay(100);
      return;
    }
    written += sent;
  }

  while(!sslSessionSocket.finish()) {
    auto err = WiFiMbedTLSSocket::lastError();
    if (err == MBEDTLS_ERR_SSL_WANT_READ || err == MBEDTLS_ERR_SSL_WANT_WRITE)
      continue;
    Serial.print("Finish failed, error: ");
    Serial.println(err);
    delay(100);
    return;
  }
}

