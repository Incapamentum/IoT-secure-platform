// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: March 29th, 2022

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
#include <SHA256.h>
#include <Transactions.h>

#include "config.h"
#include "definitions.h"

void setup() 
{
    Serial.begin(BAUD_RATE);

    Serial.println("\t\t\tSizes of specific structs/objects");
    Serial.println("================================================");
    Serial.printf("\tSize of 'Data' struct: %d (bytes)\n", sizeof(Data));
    Serial.printf("\tSize of 'Transactions' class: %d (bytes)\n", sizeof(Transaction));
}

void loop() 
{
}