// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 10th, 2022

#include <Arduino.h>
#include <Crypto.h>
#include <Ed25519.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <SimpleDHT.h>
#include <SHA256.h>
#include <stdlib.h>
#include <time.h>
#include <Transactions.h>
#include <WiFiUdp.h>

#include "config.h"
#include "definitions.h"

#define DHT11_PIN 2

//  ==============================
//              MAIN
// ===============================
char *ts_buffer;

int i, n;

String ts;
String msg;
// ===============================

//  ==============================
//             ESP8266
// ===============================
unsigned int chipId;
// ===============================

//  ==============================
//              Sensor
// ===============================
SimpleDHT11 dht11(DHT11_PIN);
byte temperature, humidity;
int err;
// ===============================

//  ==============================
//            Wifi/NTP
// ===============================
struct tm *ptm;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
// ===============================

//  ==============================
//            Crypto
// ===============================
uint8_t secretKey[KEY_LENGTH];
uint8_t publicKey[KEY_LENGTH];

uint8_t timestamp[SHA256_SIZE];
// ===============================

// Generates public-private key pair for device
void genKeys(void)
{
    Ed25519::generatePrivateKey(secretKey);
    Ed25519::derivePublicKey(publicKey, secretKey);
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

    timeClient.update();

    epochTime = timeClient.getEpochTime();

    ptm = gmtime ((time_t *)&epochTime);

    currentYear = ptm->tm_year + 1900;
    currentMonth = ptm->tm_mon + 1;
    currentDay = ptm->tm_mday;
    currentHour = timeClient.getHours();
    currentMinute = timeClient.getMinutes();
    currentSecond = timeClient.getSeconds();

    ts = String(currentYear) + "-";
    ts += String(currentMonth) + "-";
    ts += String(currentDay) + "T";
    ts += String(currentHour) + ":";
    ts += String(currentMinute) + ":";
    ts += String(currentSecond) + "+";
    ts += String(EST);
}

void keyCheck(uint8_t *ptr, String name)
{
    int i;

    Serial.print(name + " ");
    for (i = 0; i < KEY_LENGTH; i++)
        i != KEY_LENGTH - 1 ? Serial.printf("%08X ", ptr[i]) : Serial.printf(" %08X\n", ptr[i]);
}

void ntpInit(void)
{
    // Initializing NTP client
    timeClient.begin();
    timeClient.setTimeOffset(EST);
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

    Serial.printf("\nConnected\n");
}

void setup() 
{
    Serial.begin(BAUD_RATE);

    Serial.println("\tSizes of specific structs/objects");
    Serial.println("================================================");
    Serial.printf("\tSize of 'Data' struct: %d (bytes)\n", sizeof(Data));
    Serial.printf("\tSize of 'Transactions' class: %d (bytes)\n\n", sizeof(Transaction));

    // Initializations
    chipId = ESP.getChipId();
    genKeys();
    wifiInit();
    ntpInit();
}

// Might be useful to have a FSM included here
void loop() 
{
    SHA256 sha256;
    Transaction *t = new Transaction(publicKey);

    // Sensor success
    if (sampleSensor() == DHT_SUCCESS)
    {
        t->setData(temperature, humidity);

        getTime();

        msg = String(chipId) + "|" + ts;
        n = msg.length();

        ts_buffer = (char *)calloc(n, sizeof(char));
        strcpy(ts_buffer, msg.c_str());

        sha256.update(ts_buffer, n);
        sha256.finalize(timestamp, SHA256_SIZE);

        t->hashTransaction(timestamp);
        t->sign(secretKey);

        if (t->verify())
            Serial.println("Valid transaction");

        free(ts_buffer);
    }

    delay(POLL_RATE);
}