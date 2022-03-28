#include <Arduino.h>
#include <ESP8266WiFi.h>

#define BAUD_RATE 9600
#define T_DELAY 500

void setup()
{
    Serial.begin(BAUD_RATE);
    delay(T_DELAY);

    Serial.println();
    Serial.println("        ESP8266 Info");
    Serial.println("****************************");

    // MAC address
    Serial.print("WiFi MAC: ");
    Serial.println(WiFi.macAddress());

    // Chip information
    Serial.printf("Chip ID as a 32-bit integer:\t\t\t%08X\n", ESP.getChipId());
    Serial.printf("Flash chip ID as a 32-bit integer:\t\t%08X\n", ESP.getFlashChipId());
    Serial.printf("Flash chip frequency:\t\t\t\t%d (Hz)\n", ESP.getFlashChipSpeed());
    Serial.printf("Flash chip size:\t\t\t\t%d (bytes)\n", ESP.getFlashChipSize());
    Serial.printf("Free heap size:\t\t\t\t\t%d (bytes)\n", ESP.getFreeHeap());
}

void loop()
{}