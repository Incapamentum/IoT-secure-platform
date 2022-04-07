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

void setup() 
{
    
    Serial.begin(BAUD_RATE);

    Serial.println("\tSizes of specific structs/objects");
    Serial.println("================================================");
    Serial.printf("\tSize of 'Data' struct: %d (bytes)\n", sizeof(Data));
    Serial.printf("\tSize of 'Transactions' class: %d (bytes)\n", sizeof(Transaction));
}

void loop() 
{
}