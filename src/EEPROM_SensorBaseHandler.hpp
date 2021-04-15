#ifndef EEPROM_SENSORBASEHANDLER_HPP
#define EEPROM_SENSORBASEHANDLER_HPP

#include <EEPROM.h>

#include "Room.hpp"

namespace SENSOR_DEFS
{
    #define MAX_SIZE_MAC_ADDRESS    18
    #define MAX_SIZE_SENSOR_STATE   1
    #define MAX_SIZE_USERNAME       25
    #define MAX_SIZE_PASSWORD       25
    #define MAX_SIZE_SSID           25
    #define MAX_SIZE_IP_ADDRESS     4

    inline uint16_t GET_ADDRESS_OF_USERNAME(uint16_t startingAddress = 1)           { return (startingAddress);}
    inline uint16_t GET_ADDRESS_OF_PASSWORD(uint16_t startingAddress = 1)           { return (startingAddress + MAX_SIZE_USERNAME); }
    inline uint16_t GET_ADDRESS_OF_WIFI_NAME(uint16_t startingAddress = 1)          { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD); }
    inline uint16_t GET_ADDRESS_OF_WIFI_PASSWORD(uint16_t startingAddress = 1)      { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_SSID); }
    inline uint16_t GET_ADDRESS_OF_DATABASE_IP(uint16_t startingAddress = 1)        { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_SSID + MAX_SIZE_PASSWORD); }
    inline uint16_t GET_ADDRESS_OF_DADABASE_USERNAME(uint16_t startingAddress = 1)  { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_SSID + MAX_SIZE_PASSWORD + MAX_SIZE_IP_ADDRESS); }
    inline uint16_t GET_ADDRESS_OF_DADABASE_PASSWORD(uint16_t startingAddress = 1)  { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_SSID + MAX_SIZE_PASSWORD + MAX_SIZE_IP_ADDRESS + MAX_SIZE_USERNAME); }
    inline uint16_t GET_ADDRESS_OF_CONNECTED_TO_MAC(uint16_t startingAddress = 1)   { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_SSID + MAX_SIZE_PASSWORD + MAX_SIZE_IP_ADDRESS + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD); }
    inline uint16_t GET_ADDRESS_OF_ROOM(uint16_t startingAddress = 1)               { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_SSID + MAX_SIZE_PASSWORD + MAX_SIZE_IP_ADDRESS + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_MAC_ADDRESS); }
    inline uint16_t GET_ADDRESS_OF_SENSOR_STATE(uint16_t startingAddress = 1)       { return (startingAddress + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_SSID + MAX_SIZE_PASSWORD + MAX_SIZE_IP_ADDRESS + MAX_SIZE_USERNAME + MAX_SIZE_PASSWORD + MAX_SIZE_MAC_ADDRESS + Room::GET_MAX_ROOM_SIZE_IN_BYTE()); }

    /**
     * @brief ERASE_EEPROM erases all information from the eeprom.
    * 
    */
    inline void ERASE_EEPROM() 
    {
        EEPROM.begin(512);
        for (size_t i = 0; i < 512; i++)
        {
            EEPROM.writeByte(i, 0);
        }

        EEPROM.commit();
    }

    inline void printEEPROM()
    {
        EEPROM.begin(512);
        Serial.printf("Start\n");
        for (size_t i = 0; i < 512; i++)
        {
            Serial.printf("> [%3i]: %d\n", i, EEPROM.readByte(i));
        }
        Serial.printf("End\n");
    }

    // Setter
    /**
     * @brief setUsername sets the username in the eeprom.
     * 
     * @param username contains the username that is used to log in to the setup page.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setUsername(std::string username, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_USERNAME(startingAddress), username.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setUsername_oc does the same thing as setUsername but it does not commit after writing the username.
     * 
     * @param username contains the username that is used to log in to the setup page.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setUsername_oc(std::string username, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_USERNAME(startingAddress), username.c_str());
    }

    /**
     * @brief setPassword sets the password of the user in the eeprom.
     * 
     * @param password contains the password that is used to log in to the setup page.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setPassword(std::string password, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_PASSWORD(startingAddress), password.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setPassword_oc does the same thing as setPassword but it does not commit after writing the password.
     * 
     * @param password contains the password that is used to log in to the setup page.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setPassword_oc(std::string password, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_PASSWORD(startingAddress), password.c_str());
    }

    /**
     * @brief setWifiName set the name of the network that the esp32 should connect to during the runtime state in the eeprom.
     * 
     * @param wifiName contains the name of the network that the esp32 should connect to on state runtime.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setWifiName(std::string wifiName, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_WIFI_NAME(startingAddress), wifiName.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setWifiName_oc does the same thing as setWifiName but it does not commit after writing the wifi network name.
     * 
     * @param wifiName contains the name of the network that the esp32 should connect to on state runtime.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setWifiName_oc(std::string wifiName, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_WIFI_NAME(startingAddress), wifiName.c_str());
    }

    /**
     * @brief setWifiPassword sets the wifi password of the wifi network in the eeprom.
     * 
     * @param wifiPassword contains the password that is used to initiate a connection to a wifi network.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setWifiPassword(std::string wifiPassword, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_WIFI_PASSWORD(startingAddress), wifiPassword.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setWifiPassword_oc does the same thing as setWifiPassword but it does not commit after writing the wifi password.
     * 
     * @param wifiPassword contains the password that is used to initiate a connection to a wifi network.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setWifiPassword_oc(std::string wifiPassword, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_WIFI_PASSWORD(startingAddress), wifiPassword.c_str());
    }

    /**
     * @brief setDatabaseIP sets the ip address of the database that the esp32 is connected to in the eeprom.
     * 
     * @param ip contains the ip address of the database. 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setDatabaseIP(IPAddress ip, uint16_t startingAddress = 1)
    {
        EEPROM.writeInt(GET_ADDRESS_OF_DATABASE_IP(startingAddress), (uint32_t)ip);
        EEPROM.commit();
    }

    /**
     * @brief setDatabaseIP_oc does the same thing as setDatabaseIP but it does not commit after writing the database ip.
     * 
     * @param ip contains the ip address of the database. 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setDatabaseIP_oc(IPAddress ip, uint16_t startingAddress = 1)
    {
        EEPROM.writeInt(GET_ADDRESS_OF_DATABASE_IP(startingAddress), (uint32_t)ip);
    }

    /**
     * @brief setDatabaseUsername sets the username to the database that the esp32 should connect to in the epprom.
     * 
     * @param databaseLink contains the link to the database.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setDatabaseUsername(std::string databaseLink, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_DADABASE_USERNAME(startingAddress), databaseLink.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setDatabaseUsername_oc does the same thing as setDatabaseUsername but it does not commit after writing the database username.
     * 
     * @param link contains the username to the database.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setDatabaseUsername_oc(std::string link, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_DADABASE_USERNAME(startingAddress), link.c_str());
    }

    /**
     * @brief setDatabasePassword sets the database password in the eeprom.
     * 
     * @param password contains the password to the database.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setDatabasePassword(std::string password, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_DADABASE_PASSWORD(startingAddress), password.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setDatabasePassword_oc does the same thing as setDatabasePassword but it does not commit after writing the database password.
     * 
     * @param password contains the password to the database.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setDatabasePassword_oc(std::string password, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_DADABASE_PASSWORD(startingAddress), password.c_str());
    }

    /**
     * @brief setConnectedToMAC saves to what other device this esp32 is connected to in the eeprom.
     * 
     * @param connectedToMAC contains the mac address of the other device.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setConnectedToMAC(std::string connectedToMAC, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_CONNECTED_TO_MAC(startingAddress), connectedToMAC.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setConnectedToMAC_oc does the same thing as setConnectedToMAC but it does not commit after writing the connected to mac.
     * 
     * @param connectedToMAC contains the mac address of the other device.
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setConnectedToMAC_oc(std::string connectedToMAC, uint16_t startingAddress = 1)
    {
        EEPROM.writeString(GET_ADDRESS_OF_CONNECTED_TO_MAC(startingAddress), connectedToMAC.c_str());
    }

    /**
     * @brief setState sets the state if the currently running esp32 in the eeprom.
     * 
     * @param state contains the current state of the esp32.
     * @param startingAddress  is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setState(uint8_t state, uint16_t startingAddress = 1)
    {
        EEPROM.writeByte(GET_ADDRESS_OF_SENSOR_STATE(startingAddress), state);
        EEPROM.commit();
    }

    /**
     * @brief setState_oc does the same thing as setState but it does not commit after writing the sensor state.
     * 
     * @param state contains the current state of the esp32.
     * @param startingAddress  is the first byte where the sensor is located at in memory. By default it´s 0.
     */
    static inline void setState_oc(uint8_t state, uint16_t startingAddress = 1)
    {
        EEPROM.writeByte(GET_ADDRESS_OF_SENSOR_STATE(startingAddress), state);
    }

    // Getter
    /**
     * @brief getUsername returns the username from the eeprom.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the username.
     */
    static inline std::string getUsername(uint16_t startingAddress = 1)
    {
        char username[MAX_SIZE_USERNAME];
        EEPROM.readString(GET_ADDRESS_OF_USERNAME(startingAddress), username, MAX_SIZE_USERNAME);
        return username;
    }

    /**
     * @brief getPassword returns the password of the user from the eeprom.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the password.
     */
    static inline std::string getPassword(uint16_t startingAddress = 1)
    {
        char password[MAX_SIZE_PASSWORD];
        EEPROM.readString(GET_ADDRESS_OF_PASSWORD(startingAddress), password, MAX_SIZE_PASSWORD);
        return password;
    }

    /**
     * @brief getWifiName returns the wifi network name from the eeprom.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the wifi network name.
     */
    static inline std::string getWifiName(uint16_t startingAddress = 1)
    {
        char wifiName[MAX_SIZE_SSID];
        EEPROM.readString(GET_ADDRESS_OF_WIFI_NAME(startingAddress), wifiName, MAX_SIZE_SSID);
        return wifiName;
    }

    /**
     * @brief getWifiPassword returns the wifi password from the eeprom.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the wifi password.
     */
    static inline std::string getWifiPassword(uint16_t startingAddress = 1)
    {
        char wifiPassword[MAX_SIZE_PASSWORD];
        EEPROM.readString(GET_ADDRESS_OF_WIFI_PASSWORD(startingAddress), wifiPassword, MAX_SIZE_PASSWORD);
        return wifiPassword;
    }

    /**
     * @brief getDatabaseIP returns the ip address of the database from the eeprom. 
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return IPAddress of the database.
     */
    static inline IPAddress getDatabaseIP(uint16_t startingAddress = 1){
        uint32_t databaseIP = EEPROM.readInt(GET_ADDRESS_OF_DATABASE_IP(startingAddress));
        return IPAddress(databaseIP);
    }

    /**
     * @brief getDatabaseLink returns the username that is connected to the database from the esp32.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the username to the database. 
     */
    static inline std::string getDatabaseUsername(uint16_t startingAddress = 1)
    {
        char databaseLink[MAX_SIZE_USERNAME];
        EEPROM.readString(GET_ADDRESS_OF_DADABASE_USERNAME(startingAddress), databaseLink, MAX_SIZE_USERNAME);
        return databaseLink;
    }

    /**
     * @brief getDatabasePassword returns the password of the database that the esp32 is connected to from the eeprom.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the database password.
     */
    static inline std::string getDatabasePassword(uint16_t startingAddress = 1)
    {
        char databasePassword[MAX_SIZE_PASSWORD];
        EEPROM.readString(GET_ADDRESS_OF_DADABASE_PASSWORD(startingAddress), databasePassword, MAX_SIZE_PASSWORD);
        return databasePassword;
    }

    /**
     * @brief getConnectedToMAC returns the mac address of the device this esp32 is connected to from the eeprom.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the mec address of the device this eps32 is connected to.
     */
    static inline std::string getConnectedToMAC(uint16_t startingAddress = 1)
    {
        char connectedToMAC[MAX_SIZE_USERNAME];
        EEPROM.readString(GET_ADDRESS_OF_CONNECTED_TO_MAC(startingAddress), connectedToMAC, MAX_SIZE_MAC_ADDRESS);
        return connectedToMAC;
    }

    /**
     * @brief getState returns the current state of the esp32 from the eeprom.
     * 
     * @param startingAddress is the first byte where the sensor is located at in memory. By default it´s 0.
     * @return the current sensor/esp32 state.
     */
    static inline uint8_t getState(uint16_t startingAddress = 1)
    {
        return EEPROM.readByte(GET_ADDRESS_OF_SENSOR_STATE(startingAddress));
    }
}

#endif