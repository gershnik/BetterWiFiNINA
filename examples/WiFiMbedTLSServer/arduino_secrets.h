// your network SSID (name)
const char g_ssid[] = "";
// your network password (use for WPA, or use as key for WEP)
const char g_pass[] = "";

//Your certificate and private key. 
//NOTE: both must be NULL terminated strings
//even though the functions that loads them take
//size as an argument
const char g_certificate[] = 
R"(-----BEGIN CERTIFICATE-----
...your PEM encoded certificate here...
-----END CERTIFICATE-----
)";

const char g_privateKey[] = 
R"(-----BEGIN PRIVATE KEY-----
...your PEM encoded private key here...
-----END PRIVATE KEY-----
)";
