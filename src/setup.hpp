#pragma once

#include <WiFi.h>
#include <Wire.h>

#include "mqtt_defs.hpp"

/**
 * @brief setupSerial initializes the serial monitor to the ESP32. 
 */
inline void setupSerial()
{
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.printf("\n\n\n> Serial setup finished!\n");
}

/**
 * @brief setupWiFi initiates a wifi connection to a specific network.
 * 
 * @param ssid is the network to connect to.
 * @param password is the password that is neccesary to connect to the network.
 */
inline void setupWiFi(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("> Connecting to WiFi..");
    }

    Serial.printf("> Connetion to WiFi established!\n");
    Serial.printf("\t- My IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("\t- My MAC: %s\n", WiFi.macAddress().c_str());
}