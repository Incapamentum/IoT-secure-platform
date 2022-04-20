// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 20th, 2022

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
//            Iteration
// ===============================
int i, n;
// ===============================

//  ==============================
//             ESP8266
// ===============================
uint8_t chipId[sizeof(int)];
// ===============================

//  ==============================
//           Timestamps
// ===============================
char ts[23];
char *ts_buffer;

struct tm *ptm;
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
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
// ===============================

//  ==============================
//            Crypto
// ===============================
char chipStamp[STAMP_LENGTH];

uint8_t secretKey[KEY_LENGTH];
uint8_t publicKey[KEY_LENGTH];
// ===============================

// Given a starting point, copies the contents of src
// to dest
void copyDigits(char *dest, const char *src, int *start)
{
    int len = strlen(src);
    int p = 0;

    while (p < len)
        dest[(*start)++] = src[p++];
}

// Extracts the digits making up the number 'val', returning them
// as characters
char *extractDigits(int val, int num_digits)
{

    char *d_buffer;
    int digit;
    int x = num_digits - 1;

    d_buffer = (char *)calloc(num_digits + 1, sizeof(char));

    if (d_buffer == NULL)
        return NULL;

    d_buffer[num_digits] = '\0';

    while (val != 0)
    {
        digit = val % 10;
        val /= 10;
        d_buffer[x--] = digit + '0';
    }

    // Pads the rest with 0s if the value is not large enough to fill
    // in the remaining digits
    while (x >= 0)
    {
        d_buffer[x--] = '0';
    }

    return d_buffer;    
}

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
    getChipId();
    genKeys();
    wifiInit();
    ntpInit();
}

// Might be useful to have a FSM included here
void loop() 
{
    Transaction *t;
    SHA256 sha256;

    // Sensor success
    // Work on timestamp stuff
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

        if (t->verify())
            Serial.println("Valid transaction");
        else
            Serial.println("Invalid transaction");
    }

    delay(POLL_RATE);
}