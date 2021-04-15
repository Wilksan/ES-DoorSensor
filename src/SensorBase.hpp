#ifndef SENSORBASE_HPP
#define SENSORBASE_HPP

#include <string>
#include <ESPAsyncWebServer.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#include "Room.hpp"
#include "EEPROM_SensorBaseHandler.hpp"

using namespace std;

class SensorBase
{
protected:
    enum State
    {
        SETUP,
        RUNTIME,
        SLEEP,
        RESET
    };

    // Sensor stuff
    string m_deviceMAC;
    string m_connectedToMAC;
    State m_sensorState;
    Room& m_room;
    string m_description;

    // Login stuff
    string m_username;
    string m_password;

    // Network stuff
    string m_ssid;
    string m_wlanPassword;

    // Database stuff
    IPAddress m_databaseIP;
    string m_databaseUsername;
    string m_databasePassword;

    // Server
    AsyncWebServer server;

    // Database
    MySQL_Connection m_conn;
    MySQL_Cursor* m_cursor;

    // TTGO Display
    TFT_eSPI* m_tft;
    
public:
    SensorBase();
    SensorBase(TFT_eSPI* tft);
    virtual ~SensorBase();

    void saveInEEPROM(uint16_t address = 0) const;
    void loadFromEEPROM(uint16_t address = 0);
    void print() const;
    void printFromEEPROM() const;

    void toDefault();

    void setup();
    void loop();

    inline static uint8_t GET_MAX_MY_SENSOR_SIZE_IN_BYTE()
    {
        return static_cast<uint8_t>(
            1 +
            MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + 
            MAX_SIZE_SSID + MAX_SIZE_PASSWORD + 
            MAX_SIZE_IP_ADDRESS + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + 
            MAX_SIZE_MAC_ADDRESS + 
            Room::GET_MAX_ROOM_SIZE_IN_BYTE() + 
            MAX_SIZE_SENSOR_STATE);
    }
protected:
    virtual void preSetupState() = 0;
    virtual void setupSetup() = 0;
    virtual void setupRuntime() = 0;
    virtual void setupSleep() = 0;
    virtual void setupReset() = 0;

    virtual void loopSetup() = 0;
    virtual void loopRuntime() = 0;
    virtual void loopSleep() = 0;
    virtual void loopReset() = 0;
};

#endif // SENSORBASE_HPP
