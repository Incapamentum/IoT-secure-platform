#include <Arduino.h>
#include <ArduinoJson.h>
#include <Crypto.h>
#include <Ed25519.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <SHA256.h>
#include <stdlib.h>
#include <time.h>
#include <Transactions.h>
#include <WiFiUdp.h>

#include "aws_creds.h"
#include "config.h"
#include "definitions.h"

//  ==============================
//            Crypto
// ===============================
char chipStamp[STAMP_LENGTH];

uint8_t secretKey[KEY_LENGTH];
uint8_t publicKey[KEY_LENGTH];
// ===============================

// ===============================
//             ESP8266
// ===============================
uint8_t chipId[sizeof(int)];
// ===============================

// ===============================
//            Iteration
// ===============================
int i, n;
// ===============================

//  ==============================
//          PubSub Client
// ===============================
WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 
unsigned long lastPublish;
unsigned long l;
// ===============================

// ===============================
//              Sensor
// ===============================
SimpleDHT11 dht11(DHT11_PIN);
byte temperature, humidity;
int err;
// ===============================

// ===============================
//                SSL
// ===============================
BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);
// ===============================

// ===============================
//           Timestamps
// ===============================
char ts[23];
char *ts_buffer;

struct tm *ptm;
// ===============================

//  ==============================
//            Wifi/NTP
// ===============================
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
// ===============================

// Generates public-private key pair for device
void genKeys(void)
{
    Ed25519::generatePrivateKey(secretKey);
    Ed25519::derivePublicKey(publicKey, secretKey);
}

void getChipId(void)
{
    int i;
    unsigned int id;

    id = ESP.getChipId();

    for (i = sizeof(int) - 1; i >= 0; i--)
    {
        chipId[i] = id & 0xFF;
        id = id >> 8;
    }
}

void getTime(void)
{
    time_t epochTime;
    int currentYear;
    int currentMonth;
    int currentDay;
    int currentHour;
    int currentMinute;
    int currentSecond;

    // Clearing string of any data
    memset(ts, '\0', sizeof(ts));

    timeClient.update();
    epochTime = timeClient.getEpochTime();
    ptm = gmtime ((time_t *)&epochTime);

    currentYear = ptm->tm_year + 1900;
    currentMonth = ptm->tm_mon + 1;
    currentDay = ptm->tm_mday;
    currentHour = timeClient.getHours();
    currentMinute = timeClient.getMinutes();
    currentSecond = timeClient.getSeconds();

    // Writing timestamp
    snprintf(ts, sizeof(ts), "%d-%02d-%02dT%02d:%02d:%02d+%d",
        currentYear, currentMonth, currentDay, currentHour, currentMinute, currentSecond, EST);
}

void ntpInit(void)
{
    // Initializing NTP client
    timeClient.begin();
    timeClient.setTimeOffset(EST);
}

// Checks whether the client is connected to the service or not,
// and to maintain a stable connection
void pubSubCheckConnect(void)
{
    bool val;

    if ( ! pubSubClient.connected())
    {
        Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
        while ( ! pubSubClient.connected())
        {
            Serial.print(".");
            val = pubSubClient.connect("ESPthing");

            Serial.printf("Val: %d\n", val);
        }
        Serial.println(" connected");
        pubSubClient.subscribe("inTopic");
    }
    pubSubClient.loop();
}

// Handles the subscription portion of the PubSub client
void msgReceived(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
    for (unsigned int l = 0; l < length; l++)
    {
        Serial.print((char)payload[l]);
    }
    Serial.println();
}

int sampleSensor(void)
{
    Serial.println("=================================");
    Serial.println("Sample DHT11...");

    err = SimpleDHTErrSuccess;

    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    {
        Serial.println("DHT11 read failure");
        return DHT_FAILURE;
    }

    Serial.println("Sample OK");
    return DHT_SUCCESS;
}

// Configures the ESP8266 time with the current time via
// NTP access
void setCurrentTime(void)
{
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) 
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}

void wifiInit(void)
{
    // Connecting to Wifi
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(HALF_SEC);
        Serial.print(".");
    }

    Serial.printf("\nWiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(BAUD_RATE);

    Serial.println("\tSizes of specific structs/objects");
    Serial.println("================================================");
    Serial.printf("\tSize of 'Data' struct: %d (bytes)\n", sizeof(Data));
    Serial.printf("\tSize of 'Transactions' class: %d (bytes)\n\n", sizeof(Transaction));

    // Initializations
    getChipId();
    genKeys();
    wifiInit();
    ntpInit();

    // Get current time, otherwise certs will be flagged as expired
    setCurrentTime();

    wiFiClient.setClientRSACert(&client_crt, &client_key);
    wiFiClient.setTrustAnchors(&rootCert);
}

void loop() 
{
    Transaction *t;
    SHA256 sha256;
    StaticJsonDocument<BUDGET> doc;
    char *byteString;

    // Sensor success
    if (sampleSensor() == DHT_SUCCESS)
    {
        getTime();

        // Clearing string of any data
        memset(chipStamp, '\0', sizeof(chipStamp));

        // Concatinating chip ID with timestamp
        snprintf(chipStamp, sizeof(chipStamp), "%02X%02X%02X%02X|%s",
            chipId[0], chipId[1], chipId[2], chipId[3], ts);

        t = new Transaction(publicKey, chipStamp);

        t->setData(temperature, humidity);
        t->sign(secretKey);

        // The below code block may no longer be needed
        if (t->verify())
        {
            Serial.println("Valid transaction");

            doc["hum"] = t->getHumidity();
            doc["temp"] = t->getTemperature();
            doc["stamp"] = t->getStamp();
            doc["key"] = t->getOwnerKeyHex();
            doc["sign"] = t->getSignatureHex();

            serializeJson(doc, Serial);
        }
        else
            Serial.println("Invalid transaction");
    }

    delay(POLL_RATE);
}