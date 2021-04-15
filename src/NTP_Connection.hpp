#pragma once

#include <Arduino.h>
#include <time.h>

// Time and date shit
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600
#define DAYLIGHT_OFFSET_SEC 0

inline String getCurrentTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return "";
    }
    char currentTime[64];
    strftime(currentTime, sizeof(currentTime), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    return currentTime;
}

inline String getCurrentTimeAsISO8601(){
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return "";
    }
    char currentTime[9];
    strftime(currentTime, sizeof(currentTime), "%T", &timeinfo);
    return currentTime;
}

inline String getCurrentDay(){
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return "";
    }
    char currentTime[11];
    strftime(currentTime, sizeof(currentTime), "%F", &timeinfo);
    return currentTime;
}