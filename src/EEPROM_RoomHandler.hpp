#ifndef EEPROM_ROOMHANDLER_HPP
#define EEPROM_ROOMHANDLER_HPP

#include <EEPROM.h>

namespace ROOM_DEFS
{
#define MAX_SIZE_ID 2
#define MAX_SIZE_ROOM_NAME 25
#define MAX_SIZE_COMPANY_NAME 25
#define MAX_SIZE_ROOM_SIZE 2
#define MAX_SIZE_ROOM_MAX_PERSON_COUNT 1
#define MAX_SIZE_PERSON_COUNT 1

    inline uint16_t GET_ADDRESS_OF_ID(uint16_t startingAddress = 0)
    {
        return startingAddress;
    }
    inline uint16_t GET_ADDRESS_OF_COMPANY_NAME(uint16_t startingAddress = 0) { return (GET_ADDRESS_OF_ID(startingAddress) + MAX_SIZE_ID); }
    inline uint16_t GET_ADDRESS_OF_ROOM_NAME(uint16_t startingAddress = 0) { return (GET_ADDRESS_OF_ID(startingAddress) + MAX_SIZE_ID + MAX_SIZE_COMPANY_NAME); }
    inline uint16_t GET_ADDRESS_OF_ROOM_SIZE(uint16_t startingAddress = 0) { return (GET_ADDRESS_OF_ID(startingAddress) + MAX_SIZE_ID + MAX_SIZE_COMPANY_NAME + MAX_SIZE_ROOM_NAME); }
    inline uint16_t GET_ADDRESS_OF_ROOM_MAX_PERSON_COUNT(uint16_t startingAddress = 0) { return (GET_ADDRESS_OF_ID(startingAddress) + MAX_SIZE_ID + MAX_SIZE_COMPANY_NAME + MAX_SIZE_ROOM_NAME + MAX_SIZE_ROOM_SIZE); }
    inline uint16_t GET_ADDRESS_OF_PERSON_COUNT(uint16_t startingAddress = 0) { return (GET_ADDRESS_OF_ID(startingAddress) + MAX_SIZE_ID + MAX_SIZE_COMPANY_NAME + MAX_SIZE_ROOM_NAME + MAX_SIZE_ROOM_SIZE + MAX_SIZE_ROOM_MAX_PERSON_COUNT); }

    // Setter

    /**
     * @brief setID saves the id of the room into the eeprom
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param id is the id of the room.
     */
    static inline void setID(uint16_t id, uint16_t startingAddress = 0)
    {
        EEPROM.writeShort(GET_ADDRESS_OF_ID(startingAddress), id);
        EEPROM.commit();
    }

    /**
     * @brief setID_oc does the same thing as setID but it does not commit after writing the id.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param id is the id of the room.
     */
    static inline void setID_oc(uint16_t id, uint16_t startingAddress = 0)
    {
        EEPROM.writeShort(GET_ADDRESS_OF_ID(startingAddress), id);
    }

    /**
     * @brief setCompanyName saves the company name where the room phisicly is located into the eeprom.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param companyName is the name of the company.
     */
    static inline void setCompanyName(std::string companyName, uint16_t startingAddress = 0){
        EEPROM.writeString(GET_ADDRESS_OF_COMPANY_NAME(startingAddress), companyName.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setCompanyName_oc does the same thing as setCompanyName but it does not commit after writing the company name.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param companyName is the name of the company.
     */
    static inline void setCompanyName_oc(std::string companyName, uint16_t startingAddress = 0)
    {
        EEPROM.writeString(GET_ADDRESS_OF_COMPANY_NAME(startingAddress), companyName.c_str());
    }

    /**
     * @brief setName saves the name of the room in the eeprom.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param roomName is the name of the room.
     */
    static inline void setName(std::string roomName, uint16_t startingAddress = 0){
        EEPROM.writeString(GET_ADDRESS_OF_ROOM_NAME(startingAddress), roomName.c_str());
        EEPROM.commit();
    }

    /**
     * @brief setName_oc does the same thing as setName but it does not commit after writing the name.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param roomName is the name of the room.
     */
    static inline void setName_oc(std::string roomName, uint16_t startingAddress = 0)
    {
        EEPROM.writeString(GET_ADDRESS_OF_ROOM_NAME(startingAddress), roomName.c_str());
    }

    /**
     * @brief setSize saves the size of the room in the eeprom.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param size is the size of the room.
     */
    static inline void setSize(uint16_t size, uint16_t startingAddress = 0)
    {
        EEPROM.writeShort(GET_ADDRESS_OF_ROOM_SIZE(startingAddress), size);
        EEPROM.commit();
    }

    /**
     * @brief setSize_oc does the same thing as setSize_oc but it does not commit after writing the size.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param size is the size of the room.
     */
    static inline void setSize_oc(uint16_t size, uint16_t startingAddress = 0)
    {
        EEPROM.writeShort(GET_ADDRESS_OF_ROOM_SIZE(startingAddress), size);
    }

    /**
     * @brief setMaxPersonCount saves the max. person amount of the room in the eeprom.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param maxPersonCount is the max. amount of peaple that are allowed into the room.
     */
    static inline void setMaxPersonCount(uint8_t maxPersonCount, uint16_t startingAddress = 0)
    {
        EEPROM.writeByte(GET_ADDRESS_OF_ROOM_MAX_PERSON_COUNT(startingAddress), maxPersonCount);
        EEPROM.commit();
    }

    /**
     * @brief setMaxPersonCount_oc does the same thing as setMaxPersonCount but it does not commit after writing the max. person count.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param maxPersonCount is the max. amount of people that are allowed into the room.
     */
    static inline void setMaxPersonCount_oc(uint8_t maxPersonCount, uint16_t startingAddress = 0)
    {
        EEPROM.writeByte(GET_ADDRESS_OF_ROOM_MAX_PERSON_COUNT(startingAddress), maxPersonCount);
    }

    /**
     * @brief setPersonCount saves how many people are inside the room in the eeprom.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param personCount is the number of people inside the room.
     */
    static inline void setPersonCount(uint8_t personCount, uint16_t startingAddress = 0)
    {
        EEPROM.writeByte(GET_ADDRESS_OF_PERSON_COUNT(startingAddress), personCount);
        EEPROM.commit();
    }

    /**
     * @brief setPersonCount_oc does the same thing as setPersonCount but it does not commit after writing the person count.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @param personCount is the number of people inside the room.
     */
    static inline void setPersonCount_oc(uint8_t personCount, uint16_t startingAddress = 0)
    {
        EEPROM.writeByte(GET_ADDRESS_OF_PERSON_COUNT(startingAddress), personCount);
        EEPROM.commit();
    }

    // Getter

    /**
     * @brief getID returns the id of the room.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @return the id of the room. 
     */
    static inline uint16_t getID(uint16_t startingAddress = 0)
    {
        return EEPROM.readShort(GET_ADDRESS_OF_ID(startingAddress));
    }

    /**
     * @brief getCompanyName returns in what company this room is located at.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @return the company name.
     */
    static inline std::string getCompanyName(uint16_t startingAddress = 0){
        char companyNameArray[MAX_SIZE_COMPANY_NAME];
        EEPROM.readString(GET_ADDRESS_OF_COMPANY_NAME(startingAddress), companyNameArray, MAX_SIZE_COMPANY_NAME);
        return companyNameArray;
    }

    /**
     * @brief getName returns the name of the company.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @return room name.
     */
    static inline std::string getName(uint16_t startingAddress = 0)
    {
        char roomName[MAX_SIZE_ROOM_NAME];
        EEPROM.readString(GET_ADDRESS_OF_ROOM_NAME(startingAddress), roomName, MAX_SIZE_ROOM_NAME);
        return roomName;
    }

    /**
     * @brief getSize returns the size of the room.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @return the size of the room. 
     */
    static inline uint16_t getSize(uint16_t startingAddress = 0)
    {
        return EEPROM.readShort(GET_ADDRESS_OF_ROOM_SIZE(startingAddress));
    }

    /**
     * @brief getMaxPersoCount returns the max. person amount in the room saved in the eeprom.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @return the max. person count.
     */
    static inline uint8_t getMaxPersoCount(uint16_t startingAddress = 0)
    {
        return EEPROM.readByte(GET_ADDRESS_OF_ROOM_MAX_PERSON_COUNT(startingAddress));
    }

    /**
     * @brief getPersonCount returns the person count from the room saved in the eeprom.
     * 
     * @param startingAddress is the first byte where the room is located at in memory. By default it´s 0.
     * @return the person count. 
     */
    static inline uint8_t getPersonCount(uint16_t startingAddress = 0)
    {
        return EEPROM.readByte(GET_ADDRESS_OF_PERSON_COUNT(startingAddress));
    }
}
#endif