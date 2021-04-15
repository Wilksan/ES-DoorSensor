#include "Room.hpp"

Room::Room()
    : Room("", "", 0, 0, 0)
{

}

Room::Room(std::string companyName, std::string name, uint16_t size, uint8_t maxPersonCount, uint8_t personCount)
    : m_companyName(companyName),
      m_name(name),
      m_size(size),
      m_maxPersonCount(maxPersonCount),
      m_personCount(personCount)
{

}

Room::Room(Room &room)
    : Room(room.getCompanyName(), room.getName(), room.getSize(), room.getMaxPersonAmount(), room.getPersonCount())
{
    m_ID = room.m_ID;
}

void Room::publish() const
{
    // TODO
}

void Room::request()
{
    // TODO
}

uint16_t Room::getID() const
{
    return m_ID;
}

std::string Room::getCompanyName() const
{
    return m_companyName;
}
std::string Room::getName() const
{
    return m_name;
}

uint16_t Room::getSize() const
{
    return m_size;
}

uint8_t Room::getMaxPersonAmount() const
{
    return m_maxPersonCount;
}

uint8_t Room::getPersonCount() const
{
    return m_personCount;
}

void Room::setID(uint16_t id)
{
    m_ID = id;
}
void Room::setCompanyName(std::string companyName)
{
    m_companyName = companyName;
}
void Room::setName(std::string name)
{
    m_name = name;
}
void Room::setSize(uint16_t roomSize)
{
    m_size = roomSize;
}
void Room::setMaxPersonCount(uint8_t maxPersonCount)
{
    m_maxPersonCount = maxPersonCount;
}
void Room::setPersonCount(uint8_t personCount)
{
    m_personCount = personCount;
}

void Room::saveInEEPROM(uint16_t address) const
{
    ROOM_DEFS::setID_oc(m_ID, address);
    ROOM_DEFS::setCompanyName_oc(m_companyName, address);
    ROOM_DEFS::setName_oc(m_name.c_str(), address);
    ROOM_DEFS::setSize_oc(m_size, address);
    ROOM_DEFS::setMaxPersonCount_oc(m_maxPersonCount, address);
    ROOM_DEFS::setPersonCount_oc(m_personCount, address);
    EEPROM.commit();
}

void Room::loadFromEEPROM(uint16_t address)
{
    m_ID = ROOM_DEFS::getID(address);
    m_companyName = ROOM_DEFS::getCompanyName(address);
    m_name = ROOM_DEFS::getName(address);
    m_size = ROOM_DEFS::getSize(address);
    m_maxPersonCount = ROOM_DEFS::getMaxPersoCount(address);
    m_personCount = ROOM_DEFS::getPersonCount(address);
}

void Room::print() const
{
    Serial.printf("------------------ %s ------------------\n", m_name.c_str());
    Serial.printf("| ID\t\t\t|\t%d\n", m_ID);
    Serial.printf("| Company name\t\t|\t%s\n", m_companyName.c_str());
    Serial.printf("| Size\t\t\t|\t%d\n", m_size);
    Serial.printf("| Max. Person Amount\t|\t%d\n", m_maxPersonCount);
    Serial.printf("| Person count\t\t|\t%d\n", m_personCount);
    Serial.print("-----------------------------------------\n");
}
