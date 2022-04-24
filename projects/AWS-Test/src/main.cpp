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
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "aws.h"
#include "config.h"

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setCurrentTime()
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

void msgReceived(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
    for (int i = 0; i < length; i++)
    {
    Serial.print((char)payload[i]);
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200); Serial.println();
    Serial.println("ESP8266 AWS IoT Example");

    Serial.print("Connecting to "); Serial.print(ssid);
    WiFi.begin(ssid, password);
    WiFi.waitForConnectResult();
    Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

    // get current time, otherwise certificates are flagged as expired
    setCurrentTime();

    wiFiClient.setClientRSACert(&client_crt, &client_key);
    wiFiClient.setTrustAnchors(&rootCert);
}

unsigned long lastPublish;
int msgCount;

void loop() 
{
    pubSubCheckConnect();

    if (millis() - lastPublish > 10000)
    {
        String msg = String("Hello from ESP8266: ") + ++msgCount;
        pubSubClient.publish("outTopic", msg.c_str());
        Serial.print("Published: "); Serial.println(msg);
        lastPublish = millis();
    }
}