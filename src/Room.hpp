#ifndef ROOM_HPP
#define ROOM_HPP

#include <string>
#include <sstream>

#include "EEPROM_RoomHandler.hpp"

class Room{
private:
    uint16_t m_ID;
    std::string m_companyName;
    std::string m_name;
    uint16_t m_size;
    uint8_t m_maxPersonCount;
    uint8_t m_personCount;
public:
    Room();
    Room(std::string companyName, std::string name, uint16_t size, uint8_t maxPersonCount, uint8_t personCount);
    Room(Room& room);

    void publish() const;
    void request();

    // Getter
    uint16_t getID() const;
    std::string getCompanyName() const;
    std::string getName() const;
    uint16_t getSize() const;
    uint8_t getMaxPersonAmount() const;
    uint8_t getPersonCount() const;

    // Setter
    void setID(uint16_t id);
    void setCompanyName(std::string companyName);
    void setName(std::string name);
    void setSize(uint16_t roomSize);
    void setMaxPersonCount(uint8_t maxPersonAmount);
    void setPersonCount(uint8_t personCount);

    // Save in EEPROM
    void saveInEEPROM(uint16_t address) const;
    void loadFromEEPROM(uint16_t address);
    void print() const;

    // Size in byte in the EEPROM
    inline static uint16_t GET_MAX_ROOM_SIZE_IN_BYTE()
    {
        return static_cast<uint16_t>(MAX_SIZE_ID + MAX_SIZE_COMPANY_NAME + MAX_SIZE_ROOM_NAME + MAX_SIZE_ROOM_SIZE + MAX_SIZE_ROOM_MAX_PERSON_COUNT + MAX_SIZE_PERSON_COUNT);
    }
};

#endif // ROOM_HPP