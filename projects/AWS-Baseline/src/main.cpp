/* ESP8266 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP8266 with AWS IoT.
 * No messing with openssl or spiffs just regular pubsub and certificates in string constants
 * 
 * This is working as at 7th Aug 2021 with the current ESP8266 Arduino core release 3.0.2
 * 
 * Author: Anthony Elder 
 * License: Apache License v2
 */

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <SimpleDHT.h>
#include <WiFiUdp.h>

#include "aws.h"
#include "config.h"
#include "definitions.h"

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

//  ==============================
//          PubSub Client
// ===============================
char payload[DEFAULT_SIZE];
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
int err, sample;
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

long unsigned int timeStart;
long unsigned int timeEnd;
double timeElapsed;

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

// Handles the receiving of messages that is published from AWS IoT
// Basically a subscriber
void msgReceived(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
    for (int i = 0; i < length; i++)
    {
    Serial.print((char)payload[i]);
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
    Serial.printf("Sample Num: %d\n", sample++);
    return DHT_SUCCESS;
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
    sample = 1;
    Serial.begin(115200); Serial.println();
    Serial.println("ESP8266 AWS Baseline Experiment");

    // Initializations
    wifiInit();
    ntpInit();

    // get current time, otherwise certificates are flagged as expired
    setCurrentTime();

    wiFiClient.setClientRSACert(&client_crt, &client_key);
    wiFiClient.setTrustAnchors(&rootCert);
}

void loop() 
{
    StaticJsonDocument<BUDGET> doc;
    size_t size;

    // Resetting
    timeStart = 0;
    timeEnd = 0;

    pubSubCheckConnect();

    if (sampleSensor() == DHT_SUCCESS)
    {
        // Start timing
        timeStart = micros();

        getTime();

        // Clearing payload contents
        memset(payload, '\0', DEFAULT_SIZE);

        doc["temperature"] = temperature;
        doc["humidity"] = humidity;
        doc["timestamp"] = ts;

        size = serializeJson(doc, Serial);

        // Publishing
        if (millis() - lastPublish > 10000)
        {
            pubSubClient.publish("outTopic", payload, size);
            Serial.println("\nData was published");
            Serial.printf("Size of doc: %d\n", size);
            lastPublish = millis();
        }

        timeEnd = micros();
    }

    if ((timeStart != 0) && (timeEnd != 0))
    {
        timeElapsed = (double) timeEnd - timeStart;
        Serial.printf("Elapsed time: %.04f seconds\n", (double) timeElapsed / SEC_IN_MICROS);
    }

    delay(POLL_RATE);
}