#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "definitions.h"

void setup() 
{
    Serial.begin(BAUD_RATE);
    delay(500);

    Serial.println();
    Serial.println("ESP8266 Info: ");
    Serial.println("**************");

    Serial.print("WiFi MAC: ");
    Serial.println(WiFi.macAddress());

    // Chip ID
    Serial.printf("Chip ID as a 32-bit integer:\t%08X\n", ESP.getChipId());

    // Flash chip ID
    Serial.printf("Flash chip ID as a 32-bit integer:\t\t%08X\n", ESP.getFlashChipId());

    // Flash chip frequency
    Serial.printf("Flash chip frequency:\t\t\t\t%d (Hz)\n", ESP.getFlashChipSpeed());

    // Size of the flash chip
    Serial.printf("Flash chip size:\t\t\t\t%d (bytes)\n", ESP.getFlashChipSize());

    // Something about the heap size
    Serial.printf("Free heap size:\t\t\t\t\t%d (bytes)\n", ESP.getFreeHeap());

    // Should connect to the AWS server to receive it's public-private key
    // pair. Makes use of the device's unique chip ID
}

// Maybe having a state machine in here for operation may help simplify things?
void loop() 
{
}