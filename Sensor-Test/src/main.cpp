#include <Arduino.h>
#include <SimpleDHT.h>

#define DHT11_PIN 2
#define SECONDS 1.5
#define T_DELAY SECONDS * 1000

// Globals
SimpleDHT11 dht11(DHT11_PIN);

byte temperature;
byte humidity;
int err;

void setup() 
{
    Serial.begin(9600);
}

void loop() 
{
    Serial.println("=================================");
    Serial.println("Sample DHT11...");

    err = SimpleDHTErrSuccess;

    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    {
        Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
        Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(T_DELAY);
        return;
    }

    Serial.print("Sample OK: ");
    Serial.print((int)temperature); Serial.print(" *C, ");
    Serial.print((int)humidity); Serial.println(" H");

    // Sampling rate
    delay(T_DELAY);
}