// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 7th, 2022

// =========================
//           NOTES
// =========================
//
//      SHA256::finalize() takes in two params:
//          void *hash - buffer to return the hash value in
//          size_t len - length of the hash buffer (normally hashSize())

#include <Arduino.h>
#include <Crypto.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <SimpleDHT.h>
#include <Transactions.h>
#include <WiFiUdp.h>

#include "config.h"
#include "definitions.h"

#define DHT11_PIN 2

//  ==============================
//              Sensor
// ===============================
SimpleDHT11 dht11(DHT11_PIN);
int err;
// ===============================

//  ==============================
//            Wifi/NTP
// ===============================
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
// ===============================

void ntpInit(void)
{
    // Initializing NTP client
    timeClient.begin();
    timeClient.setTimeOffset(EST);
}

void sampleSensor(void)
{
    // Serial.println("=================================");
    // Serial.println("Sample DHT11...");

    // err = SimpleDHTErrSuccess;

    // if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    // {
    //     Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    //     Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(T_DELAY);
    //     return;
    // }

    // Serial.print("Sample OK: ");
    // Serial.print((int)temperature); Serial.print(" *C, ");
    // Serial.print((int)humidity); Serial.println(" H");
}

void wifiInit(void)
{
    // Connecting to Wifi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(HALF_SEC);
        Serial.print(".");
    }
}

void setup() 
{    
    Serial.begin(BAUD_RATE);

    Serial.println("\tSizes of specific structs/objects");
    Serial.println("================================================");
    Serial.printf("\tSize of 'Data' struct: %d (bytes)\n", sizeof(Data));
    Serial.printf("\tSize of 'Transactions' class: %d (bytes)\n", sizeof(Transaction));

    // Initializations
    wifiInit();
    ntpInit();
}

void loop() 
{
    // This requires the server key
    // Transaction *t = new Transaction();

    delay(POLL_RATE);
}