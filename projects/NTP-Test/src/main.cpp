// ========================
//      IMPORTANT NOTE
// ========================
//
// This project currently has some weird stack overflow(?) issue
// Will end up debugging later lmao

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "config.h"

#define BAUD_RATE 9600
#define T_DELAY 500

// Defining NTP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Week Days
String weekDays[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

// Months
String months[12] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

// Declaring globals
time_t epochTime;
String currentDate;
String currentMonthName;
String formattedTime;
String weekDay;
int currentHour;
int currentMinute;
int currentSecond;
int monthDay;
int currentMonth;
int currentYear;

struct tm *ptm;


void setup() 
{
    Serial.begin(BAUD_RATE);

    // Connect to WiFi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(T_DELAY);
        Serial.print(".");
    }

    // Initializing NTP client
    timeClient.begin();
    timeClient.setTimeOffset(-5);
}

void loop()
{
    timeClient.update();

    epochTime = timeClient.getEpochTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);

    ptm = gmtime ((time_t *)&epochTime);

    formattedTime = timeClient.getFormattedTime();
    Serial.print("Formatted Time: ");
    Serial.println(formattedTime);

    currentHour = timeClient.getHours();
    Serial.print("Hour: ");
    Serial.println(currentHour);

    currentMinute = timeClient.getMinutes();
    Serial.print("Minutes: ");
    Serial.println(currentMinute);

    currentSecond = timeClient.getSeconds();
    Serial.print("Seconds: ");
    Serial.println(currentSecond);

    weekDay = weekDays[timeClient.getDay()];
    Serial.print("Week Day: ");
    Serial.println(weekDay);

    monthDay = ptm->tm_mday;
    Serial.print("Month day: ");
    Serial.println(monthDay);

    currentMonth = ptm->tm_mon + 1;
    Serial.print("Month: ");
    Serial.println(currentMonth);

    currentMonthName = months[currentMonth + 1];
    Serial.print("Month name: ");
    Serial.println(currentMonthName);

    currentYear = ptm->tm_year + 1900;
    Serial.print("Year: ");
    Serial.println(currentYear);

    // Print complete date;
    currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
    Serial.print("Current date: ");
    Serial.println(currentDate);

    Serial.println("");

    delay(4 * T_DELAY);
}