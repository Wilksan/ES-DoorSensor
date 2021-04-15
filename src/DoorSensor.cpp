#include "DoorSensor.hpp"

#include <vector>
#include <string>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESP32Tone.h>

#include "DatabaseHelper.hpp"
#include "NTP_Connection.hpp"
#include "pinout.h"

using namespace std;

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522;

void increaseDatabaseRoomPersonCount(MySQL_Connection *conn, Room *room);
void decreaseDatabaseRoomPersonCount(MySQL_Connection *conn, Room *room);

void turnRed();
void turnYellow();
void turnGreen();

void doPositiveSound();
void doNegativeSound();

DoorSensor::DoorSensor()
    : SensorBase()
{
}

DoorSensor::DoorSensor(TFT_eSPI *tft)
    : SensorBase(tft)
{
}

DoorSensor::~DoorSensor()
{

}

void DoorSensor::preSetupState()
{
    
}

void DoorSensor::setupSetup()
{
}

void DoorSensor::setupRuntime()
{
    // MFRC522 setup
    SPI.begin(PIN_SCK, PIN_MISO_CUSTOM, PIN_MOSI_CUSTOM, PIN_SS);
    mfrc522.PCD_Init(PIN_SS, PIN_RST);
    mfrc522.PCD_DumpVersionToSerial();

    // I2C LCD display setup
    Wire.begin(PIN_LCD_SDA, PIN_LCD_SCL);

    lcd.init();
    lcd.backlight();
    lcd.clear();

    pinMode(PIN_AMPEL_RED, OUTPUT);
    pinMode(PIN_AMPEL_YELLOW, OUTPUT);
    pinMode(PIN_AMPEL_GREEN, OUTPUT);
    pinMode(PIN_LAUTSPRECHER, OUTPUT);
    pinMode(PIN_PIEZO, OUTPUT);

    digitalWrite(PIN_AMPEL_RED, LOW);
    digitalWrite(PIN_AMPEL_YELLOW, LOW);
    digitalWrite(PIN_AMPEL_GREEN, LOW);

    m_tft->fillScreen(TFT_BLACK);
    m_tft->setCursor(28, 50);
    m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
    m_tft->setTextSize(4);
    m_tft->printf("%3d/%3d\n", (int)m_room.getPersonCount(), (int)m_room.getMaxPersonAmount());

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Eingang");
    lcd.setCursor(0, 1);
    lcd.println("Nutze RFID Karte");
}

void DoorSensor::setupSleep()
{
    
}

void DoorSensor::setupReset()
{
    
}

void DoorSensor::loopSetup()
{

}

int oldRoomPersonCountValue = 0;

void DoorSensor::loopRuntime()
{
    turnYellow();

    // Only repaint the person count if something changed
    if (oldRoomPersonCountValue != m_room.getPersonCount())
    {
        m_tft->fillScreen(TFT_BLACK);
        m_tft->setCursor(28, 50);
        m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
        m_tft->setTextSize(4);
        m_tft->printf("%3d/%3d\n", (int)m_room.getPersonCount(), (int)m_room.getMaxPersonAmount());
    }

    oldRoomPersonCountValue = m_room.getPersonCount();

    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    lcd.clear();

    // Dump debug info about the card; PICC_HaltA() is automatically called
    //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

    char buffer[8];
    sprintf(buffer, "%02X%02X%02X%02X", mfrc522.uid.uidByte[0], mfrc522.uid.uidByte[1], mfrc522.uid.uidByte[2], mfrc522.uid.uidByte[3]);
    Serial.printf("> RFID: %s\n", buffer);

    lcd.setCursor(0, 1);
    lcd.printf("RFID:%s", buffer);

    std::stringstream ss;
    ss << buffer;
    std::string rfid = ss.str();

    std::string currentDay = getCurrentDay().c_str();
    std::string currentTime = getCurrentTimeAsISO8601().c_str();
    std::string macAddress = WiFi.macAddress().c_str();

    int hat_betretenRFIDCount = 0;
    int hat_verlassenRFIDCount = 0;
    int roomPersonCount = 0;

    // Establish a connection to the database server.
    if (!m_conn.connected())
        connectToDatabase(m_conn, m_databaseIP, m_databaseUsername.c_str(), m_databasePassword.c_str());

    {
        // Find out how often the given rfid can be found the the hat_betreten and hat_verlassen database.
        char bufferCondition[64];
        DATABASE_GENERATE_CONDITION("%s = '%s'", "NutzerRFID", rfid.c_str(), bufferCondition, 64);
        hat_betretenRFIDCount = atoi(getDatabaseSelectResponse(&m_conn, "es_datenbank.hat_betreten", "COUNT(NutzerRFID)", bufferCondition).at(0).c_str());
        hat_verlassenRFIDCount = atoi(getDatabaseSelectResponse(&m_conn, "es_datenbank.hat_verlassen", "COUNT(NutzerRFID)", bufferCondition).at(0).c_str());

    }

    m_cursor = new MySQL_Cursor(&m_conn);
    {
        char bufferDatabase[256];
        char bufferDatabaseValues[64];

        sprintf(bufferDatabaseValues, "('%s', %d, '%s', '%s', '%s')", rfid.c_str(), m_room.getID(), macAddress.c_str(), currentDay.c_str(), currentTime.c_str());

        // If the rfid is more often in the hat_betreten database table then in the hat_verlassen database table
        // then this mean that the user is leaving the rome. Otherwise the user is entering the room.
        // Based on this occupation generate the equivilant database INSERT INTO command.
        if (hat_betretenRFIDCount > hat_verlassenRFIDCount)
        {
            DATABASE_INSERT(
                "es_datenbank.hat_verlassen",
                "(NutzerRFID, RaeumeID, GeraeteMAC, Datum, Uhrzeit)",
                bufferDatabaseValues,
                bufferDatabase,
                256
            );
            decreaseDatabaseRoomPersonCount(&m_conn, &m_room);

            Serial.printf("[D]> %s\n", bufferDatabase);
            m_cursor->execute(bufferDatabase);
            turnGreen();
        }
        else
        { 
            // A user can only enter a room when the max. person threshhold is not exceedet
            if (m_room.getPersonCount() < m_room.getMaxPersonAmount())
            {
                DATABASE_INSERT(
                    "es_datenbank.hat_betreten",
                    "(NutzerRFID, RaeumeID, GeraeteMAC, Datum, Uhrzeit)",
                    bufferDatabaseValues,
                    bufferDatabase,
                    256
                );
                increaseDatabaseRoomPersonCount(&m_conn, &m_room);
                
                Serial.printf("[D]> %s\n", bufferDatabase);
                m_cursor->execute(bufferDatabase);

                lcd.setCursor(0, 0);
                lcd.printf("Bitte Eintreten!");
                turnGreen();
                doPositiveSound();
            }
            else
            {
                lcd.setCursor(0, 0);
                lcd.printf("Nicht eintreten!");
                turnRed();
                doNegativeSound();
            }
        }
    }

    {
        char bufferCondition[64];
        DATABASE_GENERATE_CONDITION("%s = %d", "ID", m_room.getID(), bufferCondition, 64);
        roomPersonCount = atoi(getDatabaseSelectResponse(&m_conn, "es_datenbank.raeume", "PersonenAnzahl", bufferCondition).at(0).c_str());
    }
    m_room.setPersonCount(roomPersonCount);

    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Eingang");
    lcd.setCursor(0, 1);
    lcd.println("Nutze RFID Karte");

    m_cursor->close();
}

void DoorSensor::loopSleep()
{
    
}

void DoorSensor::loopReset()
{
    
}

void increaseDatabaseRoomPersonCount(MySQL_Connection* conn, Room* room)
{
    MySQL_Cursor* cursor = new MySQL_Cursor(conn);

    char buffer[256];
    char bufferCondition[64];
    char bufferValues[64];

    DATABASE_GENERATE_CONDITION(
        "%s = %d",
        "ID",
        room->getID(),
        bufferCondition,
        64
    );

    sprintf(
        bufferValues,
        "PersonenAnzahl = %d",
        (room->getPersonCount() < room->getMaxPersonAmount()) ? room->getPersonCount() + 1 : room->getMaxPersonAmount());

    DATABASE_UPDATE(
        "es_datenbank.raeume",
        bufferValues,
        bufferCondition,
        buffer,
        256
    );

    printDatabaseCommand(buffer, __LINE__, __FILE__);
    cursor->execute(buffer);
    cursor->close();
}
void decreaseDatabaseRoomPersonCount(MySQL_Connection *conn, Room *room)
{
    MySQL_Cursor *cursor = new MySQL_Cursor(conn);

    char buffer[256];
    char bufferCondition[64];
    char bufferValues[64];

    DATABASE_GENERATE_CONDITION(
        "%s = %d",
        "ID",
        room->getID(),
        bufferCondition,
        64);

    sprintf(
        bufferValues,
        "PersonenAnzahl = %d",
        (room->getPersonCount() > 0) ? room->getPersonCount() - 1 : 0);

    DATABASE_UPDATE(
        "es_datenbank.raeume",
        bufferValues,
        bufferCondition,
        buffer,
        256);

    printDatabaseCommand(buffer, __LINE__, __FILE__);
    cursor->execute(buffer);
    cursor->close();
}

void turnRed()
{
    digitalWrite(PIN_AMPEL_RED, HIGH);
    digitalWrite(PIN_AMPEL_YELLOW, LOW);
    digitalWrite(PIN_AMPEL_GREEN, LOW);
}
void turnYellow()
{
    digitalWrite(PIN_AMPEL_RED, LOW);
    digitalWrite(PIN_AMPEL_YELLOW, HIGH);
    digitalWrite(PIN_AMPEL_GREEN, LOW);
}
void turnGreen()
{
    digitalWrite(PIN_AMPEL_RED, LOW);
    digitalWrite(PIN_AMPEL_YELLOW, LOW);
    digitalWrite(PIN_AMPEL_GREEN, HIGH);
}

void doNegativeSound()
{
    digitalWrite(PIN_PIEZO, HIGH);
    delay(500);
    digitalWrite(PIN_PIEZO, LOW);
    delay(500);
    digitalWrite(PIN_PIEZO, HIGH);
    delay(500);
    digitalWrite(PIN_PIEZO, LOW);
}
void doPositiveSound()
{
    tone(PIN_LAUTSPRECHER, 3400);
    delay(300);
    tone(PIN_LAUTSPRECHER, 3540);
    delay(300);
    noTone(PIN_LAUTSPRECHER);
}