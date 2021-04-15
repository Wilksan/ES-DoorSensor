#include "SensorBase.hpp"

#include <SPIFFS.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "setup.hpp"
#include "NTP_Connection.hpp"
#include "DebugMode.h"
#include "DatabaseHelper.hpp"

// RTC

using namespace std;

String processLogin(const String& var);
String processConfig(const String& var);

String processLoginPost(const String& var);

void requestRoomDataFromDatabase(MySQL_Connection *conn, Room *room);

Room room;

// WiFi
WiFiClient espClient;

SensorBase::SensorBase()
    : m_deviceMAC(""),
      m_connectedToMAC(""),
      m_sensorState(State::SETUP),
      m_room(room),
      m_description("Tuerensensor"),
      m_username("admin"),
      m_password("012345678"),
      m_ssid(""),
      m_wlanPassword(""),
      m_databaseIP(192, 168, 178, 82),
      m_databaseUsername("esp32User"),
      m_databasePassword("Qwertz1998"), // jGXjVuVhY4vHOc1r
      server(AsyncWebServer(80)),
      m_conn((Client *)&espClient),
      m_cursor(new MySQL_Cursor(&m_conn))
{
}

SensorBase::SensorBase(TFT_eSPI *tft)
    : SensorBase()
{
    m_tft = tft;
}

SensorBase::~SensorBase()
{
    saveInEEPROM();
    if(m_cursor != nullptr)
    {
        m_cursor->close();
    }
    if(m_conn.connected())
    {
        m_conn.close();
    }
    delete m_cursor;
}

void SensorBase::setup()
{
    // Serial monitor setup
    setupSerial();

    // RUN THIS FUNCTION ONLY ON VERY FIRST START!
            SENSOR_DEFS::ERASE_EEPROM();

    // Loading sensor properties from memory
    EEPROM.begin(GET_MAX_MY_SENSOR_SIZE_IN_BYTE());

    // Only load data from EEPROM if the first by is written
    // otherwise reset the EEPROM to 0.
    if(EEPROM.readUChar(0) < (uint8_t)255)
    {
        Serial.println("> First byte is not 255!\n> Reseting ESP32!");
        SENSOR_DEFS::ERASE_EEPROM();
    }else
    {
        loadFromEEPROM();
    }

    Serial.println("> Setup SPIFFS!");
    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("\t- An Error has occurred while mounting SPIFFS");
        return;
    }
    Serial.println("> SPIFFS setup finished!");

    // Initialize all the sensor specifc stuff, 
    // that does not belong into the state machine
    preSetupState();

    m_tft->init();
    m_tft->setRotation(1);
    m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
    m_tft->fillScreen(TFT_BLACK);

    switch (m_sensorState)
    {
    case State::SETUP:
    {
        saveInEEPROM();
        printFromEEPROM();

#ifdef DEBUG_MODE
        // The sensor becomes its own wifi router
        setupWiFi(DEBUG_MODE_WLAN_SSID, DEBUG_MODE_WLAN_PASSWORD);
#else
        // Initialize WiFi
        WiFi.softAP(m_ssid.c_str(), m_wlanPassword.c_str());
        // Host WiFi name
        char buffer[256] = {};
        sprintf(buffer, "RoomOwl%s%s", this->m_description.c_str(), this->m_deviceMAC.c_str());
        WiFi.setHostname(buffer);
        Serial.println(WiFi.getHostname());
#endif

        // Print the IP-Adress that the user has to connect to
        // to the display
        IPAddress ip = WiFi.localIP();
        m_tft->setCursor(0, 0);
        m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
        m_tft->setTextSize(2);
        m_tft->printf("IP:\n%s\n", ip.toString().c_str());

        setupSetup();

        // Route for login page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/login.html", String(), false, processLogin);
        });

        // Route to load style.css file
        server.on("/css/login.css", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/css/login.css", "text/css");
        });

        // Route to load js file
        server.on("/js/login.js", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/js/login.js", "text/javascript");
        });

        // Responde to login post request
        server.on(
            "/api/posts/login",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [this] (AsyncWebServerRequest * request, uint8_t * data, size_t len, size_t index, size_t total){
                char* loginData = reinterpret_cast<char*>(data);
                DynamicJsonDocument doc(512);
                deserializeJson(doc, loginData);

                std::string username = doc["username"];
                std::string password = doc["password"];

                if ((username.compare(SENSOR_DEFS::getUsername().c_str()) == 0) && (password.compare(SENSOR_DEFS::getPassword().c_str()) == 0))
                {
                    request->send(200, "text/plain", "true");
                    String msg = "Richtig!";
                    Serial.printf("<-> %s, %s => %s\n", username.c_str(), password.c_str(), msg.c_str());
                }
                else
                {
                    request->send(200, "text/plain", "false");
                    String msg = "Falsch!";
                    Serial.printf("<-> %s, %s => %s\n", username.c_str(), password.c_str(), msg.c_str());
                }
            }
        );

        // Send the username to the setup page on request
        server.on(
            "/html/redirect/api/posts/userdata",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                
                AsyncJsonResponse *response = new AsyncJsonResponse();
                response->addHeader("Server", "Requested user data");
                JsonObject root = response->getRoot();
                root["username"] = SENSOR_DEFS::getUsername();
                root["password"] = SENSOR_DEFS::getPassword();
                response->setLength();

                Serial.println("<--> Uset data requested!");

                this->loadFromEEPROM();
                this->print();

                request->send(response);
            });

        // Route to new_login_info_setup page
        server.on("/html/redirect", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/html/setup.html", String(), false, processConfig);
        });

        // Route to load style.css file
        server.on("/css/setup.css", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/css/setup.css", "text/css");
        });

        // Route to load js file
        server.on("/js/setup.js", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/js/setup.js", "text/javascript");
        });

        // Route to the icon png
        server.on("/rsc/icon.png", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/rsc/icon.png", "image/png");
        });

        // Handle the recieved new user data from the client
        server.on(
            "/html/redirect/api/posts/new_userdata",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                char *loginData = reinterpret_cast<char *>(data);
                DynamicJsonDocument doc(512);
                deserializeJson(doc, loginData);

                Serial.println("\n -> Data for the user recieved:");

                std::string oldUsername = SENSOR_DEFS::getUsername();
                SENSOR_DEFS::setUsername(doc["username"]);
                this->m_username = static_cast<const char*>(doc["username"]);
                Serial.printf("\t\t<-> Username: %s => %s\n", oldUsername.c_str(), SENSOR_DEFS::getUsername().c_str());

                std::string oldPassword = SENSOR_DEFS::getPassword();
                SENSOR_DEFS::setPassword(doc["password"]);
                this->m_password = static_cast<const char *> (doc["password"]);
                Serial.printf("\t\t<-> Password: %s => %s\n", oldPassword.c_str(), SENSOR_DEFS::getPassword().c_str());

                this->print();

                Serial.println("=====================================================\n\n");

                request->send(200);
            });

        // Handle the recieved new wifi data from the client
        server.on(
            "/html/redirect/api/posts/new_wifi_data",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                char *loginData = reinterpret_cast<char *>(data);
                DynamicJsonDocument doc(512);
                deserializeJson(doc, loginData);

                Serial.println("\n -> Data for the wifi recieved:");

                std::string oldWifiName = SENSOR_DEFS::getWifiName();
                SENSOR_DEFS::setWifiName(doc["wifi_name"]);
                this->m_ssid = static_cast<const char*>(doc["wifi_name"]);
                Serial.printf("\t\t<-> Network name: %s => %s\n", oldWifiName.c_str(), SENSOR_DEFS::getWifiName().c_str());

                std::string oldWifiPassword = SENSOR_DEFS::getWifiPassword();
                SENSOR_DEFS::setWifiPassword(doc["wifi_password"]);
                this->m_wlanPassword = static_cast<const char*>(doc["wifi_password"]);
                Serial.printf("\t\t<-> Password: %s => %s\n", oldWifiPassword.c_str(), SENSOR_DEFS::getWifiPassword().c_str());

                this->print();

                Serial.println("=====================================================\n\n");

                request->send(200);
            });

        // Handle the recieved new room data from the client
        server.on(
            "/html/redirect/api/posts/new_room_data",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                char *loginData = reinterpret_cast<char *>(data);
                DynamicJsonDocument doc(512);
                deserializeJson(doc, loginData);

                Serial.println("\n -> Data for the room recieved:");

                std::string oldCompanyName = ROOM_DEFS::getCompanyName(SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                ROOM_DEFS::setCompanyName(doc["companyName"], SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                this->m_room.setCompanyName(doc["companyName"]);
                Serial.printf("\t\t<-> Company: %s => %s\n", oldCompanyName.c_str(), ROOM_DEFS::getCompanyName(SENSOR_DEFS::GET_ADDRESS_OF_ROOM()).c_str());

                std::string oldRoomName = ROOM_DEFS::getName(SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                ROOM_DEFS::setName(doc["roomName"], SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                this->m_room.setName(doc["roomName"]);
                Serial.printf("\t\t<-> Room name: %s => %s\n", oldRoomName.c_str(), ROOM_DEFS::getName(SENSOR_DEFS::GET_ADDRESS_OF_ROOM()).c_str());

                uint16_t oldRoomSize = ROOM_DEFS::getSize(SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                ROOM_DEFS::setSize(std::atoi(doc["roomSize"]), SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                this->m_room.setSize(std::atoi(doc["roomSize"]));
                Serial.printf("\t\t<-> Room size: %d => %d\n", oldRoomSize, ROOM_DEFS::getSize(SENSOR_DEFS::GET_ADDRESS_OF_ROOM()));

                uint16_t oldMaxPersonCount = ROOM_DEFS::getMaxPersoCount(SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                ROOM_DEFS::setMaxPersonCount(std::atoi(doc["maxRoomSersonCount"]), SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                this->m_room.setMaxPersonCount(atoi(doc["maxRoomSersonCount"]));
                Serial.printf("\t\t<-> Max. person count: %d => %d\n", oldMaxPersonCount, ROOM_DEFS::getMaxPersoCount(SENSOR_DEFS::GET_ADDRESS_OF_ROOM()));

                // Push the room information into the database
                // If the room already exists then alter the already existing room

                if(!m_conn.connected())
                    connectToDatabase(m_conn, m_databaseIP, m_databaseUsername.c_str(), m_databasePassword.c_str());

                MySQL_Cursor* cursor = new MySQL_Cursor(&m_conn);

                char buffer[256];
                char bufferCondition[64];

                std::string roomName = ROOM_DEFS::getName(SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
                
                sprintf(
                    bufferCondition, 
                    "%s = '%s'", 
                    "Name", 
                    roomName.c_str()
                );

                std::vector<std::string> selectResponse = getDatabaseSelectResponse(
                    &m_conn,
                    "es_datenbank.raeume",
                    "Name",
                    bufferCondition
                );

                if (selectResponse.size() == 0)
                {
                    // Count how many rooms there are to get the next id
                    selectResponse = getDatabaseSelectResponse(
                        &m_conn,
                        "es_datenbank.raeume",
                        "COUNT(ID)",
                        "1"
                    );

                    int roomsCount = atoi(selectResponse.at(0).c_str());
                    Serial.printf("> [S]: Room id set to: %d\n", roomsCount);
                    m_room.setID(roomsCount);
                    ROOM_DEFS::setID(roomsCount, SENSOR_DEFS::GET_ADDRESS_OF_ROOM());

                    // Push all room information to the database
                    char* bufferValues = new char[128];

                    sprintf(
                        bufferValues,
                        "(%d, '%s', %d, %d)",
                        m_room.getID(),
                        m_room.getName().c_str(),
                        m_room.getSize(),
                        m_room.getMaxPersonAmount()
                    );

                    DATABASE_INSERT(
                        "es_datenbank.raeume",
                        "(ID, Name, RaumGroesse, MaxPersonenAnzahl)",
                        bufferValues,
                        buffer,
                        256
                    );

                    printDatabaseCommand(buffer, __LINE__, __FILE__);
                    cursor->execute(buffer);
                    delete bufferValues;
                }else
                {
                    selectResponse = getDatabaseSelectResponse(
                        &m_conn,
                        "es_datenbank.raeume",
                        "ID",
                        bufferCondition
                    );

                    int roomsCount = atoi(selectResponse.at(0).c_str());
                    m_room.setID(roomsCount);
                    ROOM_DEFS::setID(roomsCount, SENSOR_DEFS::GET_ADDRESS_OF_ROOM());

                    // Alter the already existing table
                    char *bufferValues = new char[128];

                    sprintf(
                        bufferValues,
                        "ID = %d, Name = '%s', RaumGroesse = %d, MaxPersonenAnzahl = %d",
                        m_room.getID(),
                        m_room.getName().c_str(),
                        m_room.getSize(),
                        m_room.getMaxPersonAmount()
                    );

                    DATABASE_UPDATE(
                        "es_datenbank.raeume", 
                        bufferValues, 
                        bufferCondition, 
                        buffer, 
                        256
                    );

                    printDatabaseCommand(buffer, __LINE__, __FILE__);
                    cursor->execute(buffer);
                    delete bufferValues;
                }

                request->send(200);
                cursor->close();

                Serial.println("=====================================================\n\n");
                this->m_room.print();
                Serial.println("=====================================================\n\n");
            });

        server.on(
            "/html/redirect/api/posts/new_database_data",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                char *loginData = reinterpret_cast<char *>(data);
                DynamicJsonDocument doc(512);
                deserializeJson(doc, loginData);

                Serial.println("\n -> Data for the database recieved:");

                std::string oldDatabaseIP = SENSOR_DEFS::getDatabaseIP().toString().c_str();
                IPAddress databaseIP;

                databaseIP[0] = atoi(doc["ip"]["byte_1"]);
                databaseIP[1] = atoi(doc["ip"]["byte_2"]);
                databaseIP[2] = atoi(doc["ip"]["byte_3"]);
                databaseIP[3] = atoi(doc["ip"]["byte_4"]);

                SENSOR_DEFS::setDatabaseIP(databaseIP);
                this->m_databaseIP = databaseIP;
                Serial.printf("\t\t<-> IP: %s => %s\n", oldDatabaseIP.c_str(), SENSOR_DEFS::getDatabaseIP().toString().c_str());

                std::string oldDatabaseUsername = SENSOR_DEFS::getDatabaseUsername();
                SENSOR_DEFS::setDatabaseUsername(doc["username"]);
                this->m_databaseUsername = static_cast<const char*>(doc["username"]);
                Serial.printf("\t\t<-> Username: %s => %s\n", oldDatabaseUsername.c_str(), SENSOR_DEFS::getDatabaseUsername().c_str());

                std::string oldDatabasePassword = SENSOR_DEFS::getDatabasePassword();
                SENSOR_DEFS::setDatabasePassword(doc["password"]);
                this->m_databasePassword = static_cast<const char*>(doc["password"]);
                Serial.printf("\t\t<-> Password: %s => %s\n", oldDatabasePassword.c_str(), SENSOR_DEFS::getDatabasePassword().c_str());

                this->print();
                
                Serial.println("=====================================================\n\n");

                request->send(200);
            });

        server.on("/html/redirect/api/posts/commit", HTTP_POST, [this](AsyncWebServerRequest *request) {
            
            Serial.println("> ESP32 commit requested!");

            // Establish the database connection
            if(!m_conn.connected())
                connectToDatabase(m_conn, m_databaseIP, m_databaseUsername.c_str(), m_databasePassword.c_str());

            MySQL_Cursor *cursor = new MySQL_Cursor(&m_conn);

            char buffer[256];
            char bufferCondition[64];
           
            // Send device data to the database
            {
                DATABASE_GENERATE_CONDITION("%s = '%s'", "MAC", WiFi.macAddress().c_str(), bufferCondition, 64);
                vector<string> selectResponse = getDatabaseSelectResponse(
                    &m_conn,
                    "es_datenbank.geraete",
                    "MAC",
                    bufferCondition
                );

                if (selectResponse.size() > 0)
                {
                    BUFFER_TO_DEFAULT_VALUE(buffer, 256);
                    sprintf(
                        buffer,
                        "UPDATE es_datenbank.geraete SET MAC = '%s', Bezeichnung = '%s' WHERE %s",
                        WiFi.macAddress().c_str(), m_description.c_str(),
                        bufferCondition);

                    printDatabaseCommand(buffer, __LINE__, __FILE__);
                    cursor->execute(buffer);
                }
                else
                {
                    char *bufferValues = new char[128];
                    sprintf(bufferValues, "('%s', '%s')", WiFi.macAddress().c_str(), m_description.c_str());
                    DATABASE_INSERT("es_datenbank.geraete", "(MAC, Bezeichnung)", bufferValues, buffer, 256);

                    printDatabaseCommand(buffer, __LINE__, __FILE__);
                    cursor->execute(buffer);
                    delete bufferValues;
                }
            }

            delay(500);

            cursor->close();
            m_conn.close();

            // Send feedback
            request->send(200);

            delay(2000);

            this->m_sensorState = State::RUNTIME;
            saveInEEPROM();

            ESP.restart();
        });

        server.begin();
    }
        break;
    case State::RUNTIME:
        {
            // Load settings from memory
            loadFromEEPROM();
            printFromEEPROM();

#ifdef DEBUG_MODE
            // Initialize WiFi
            setupWiFi(DEBUG_MODE_WLAN_SSID, DEBUG_MODE_WLAN_PASSWORD);
#else
            // The sensor becomes its own wifi router
            WiFi.softAP(m_ssid.c_str(), m_wlanPassword.c_str());
            // Host WiFi name
            char buffer[256] = {};
            sprintf(buffer, "RoomOwl%s%s", this->m_description.c_str(), this->m_deviceMAC.c_str());
            WiFi.setHostname(buffer);
            Serial.println(WiFi.getHostname());
#endif

            // Get local time from ntp server
            configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

            setupRuntime();

            // Send the login page on request
            server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
                request->send(SPIFFS, "/login.html", String(), false, processLogin);
            });

            // Send the style.css file on request
            server.on("/css/login.css", HTTP_GET, [](AsyncWebServerRequest *request) {
                request->send(SPIFFS, "/css/login.css", "text/css");
            });

            // Send the js file on request
            server.on("/js/login.js", HTTP_GET, [](AsyncWebServerRequest *request) {
                request->send(SPIFFS, "/js/login.js", "text/javascript");
            });

            // Route to the icon png
            server.on("/rsc/icon.png", HTTP_GET, [](AsyncWebServerRequest *request) {
                request->send(SPIFFS, "/../rsc/icon.png", "image/png");
            });

            // Send the reset page on request
            server.on("/html/redirect", HTTP_GET, [](AsyncWebServerRequest *request){
                request->send(SPIFFS, "/html/reset.html", String(), false, processConfig);
            });

            // Send the reset.css file on request
            server.on("/css/reset.css", HTTP_GET, [](AsyncWebServerRequest *request) {
                request->send(SPIFFS, "/css/reset.css", "text/css");
            });

            // Send the js file on request
            server.on("/js/reset.js", HTTP_GET, [](AsyncWebServerRequest *request) {
                request->send(SPIFFS, "/js/reset.js", "text/javascript");
            });

            server.on("/html/redirect/api/posts/reset", HTTP_POST, [this](AsyncWebServerRequest *request) {
                for(size_t i = 0; i < GET_MAX_MY_SENSOR_SIZE_IN_BYTE(); i++)
                {
                    EEPROM.writeByte(i, 0);
                }
                EEPROM.commit();

                delay(500);

                server.end();
                request->send(200);

                this->toDefault();
                saveInEEPROM();

                delay(1000);
                ESP.restart();
            });

            // Responde to login post request
            server.on(
                "/api/posts/login",
                HTTP_POST,
                [](AsyncWebServerRequest *request) {},
                NULL,
                [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                char *loginData = reinterpret_cast<char *>(data);
                DynamicJsonDocument doc(512);
                deserializeJson(doc, loginData);

                std::string username = doc["username"];
                std::string password = doc["password"];

                if ((username.compare(SENSOR_DEFS::getUsername().c_str()) == 0) && (password.compare(SENSOR_DEFS::getPassword().c_str()) == 0))
                {
                    request->send(200, "text/plain", "true");
                    String msg = "Richtig!";
                    Serial.printf("<-> %s, %s => %s\n", username.c_str(), password.c_str(), msg.c_str());
                }
                else
                {
                    request->send(200, "text/plain", "false");
                    String msg = "Falsch!";
                    Serial.printf("<-> %s, %s => %s\n", username.c_str(), password.c_str(), msg.c_str());
                }
            });

            server.begin();

            requestRoomDataFromDatabase(&m_conn, &m_room);
        }
        break;
    case State::SLEEP:
        setupSleep();       
        break;
    case State::RESET:
        {
            setupReset();
        }
        break;
    default:
        break;
    }
}

void SensorBase::loop()
{
    switch (m_sensorState)
    {
    case State::SETUP:
    {
        loopSetup();
    }
        break;
    case State::RUNTIME:
    {
        loopRuntime();
    }
        break;
    case State::SLEEP:
    {
        loopSleep();
    }
        break;
    case State::RESET:
    {
        loopReset();
    }
        break;
    default:
        break;
    }
}

void SensorBase::saveInEEPROM(uint16_t address) const
{
    EEPROM.writeByte(0, (uint8_t)255);

    uint16_t newAddres = address + 1;
    SENSOR_DEFS::setUsername_oc(m_username, newAddres);
    SENSOR_DEFS::setPassword_oc(m_password, newAddres);
    SENSOR_DEFS::setWifiName_oc(m_ssid, newAddres);
    SENSOR_DEFS::setWifiPassword_oc(m_wlanPassword, newAddres);
    SENSOR_DEFS::setDatabaseIP_oc(m_databaseIP, newAddres);
    SENSOR_DEFS::setDatabaseUsername_oc(m_databaseUsername, newAddres);
    SENSOR_DEFS::setDatabasePassword_oc(m_databasePassword, newAddres);
    SENSOR_DEFS::setConnectedToMAC_oc(m_connectedToMAC, newAddres);
    m_room.saveInEEPROM(SENSOR_DEFS::GET_ADDRESS_OF_ROOM(newAddres));
    SENSOR_DEFS::setState_oc(m_sensorState, newAddres);
    EEPROM.commit();
}

void SensorBase::loadFromEEPROM(uint16_t address)
{
    uint16_t newAddres = address + 1;
    m_username = SENSOR_DEFS::getUsername(newAddres);
    m_password = SENSOR_DEFS::getPassword(newAddres);
    m_ssid = SENSOR_DEFS::getWifiName(newAddres);
    m_wlanPassword = SENSOR_DEFS::getWifiPassword(newAddres);
    m_databaseIP = SENSOR_DEFS::getDatabaseIP(newAddres);
    m_databaseUsername = SENSOR_DEFS::getDatabaseUsername(newAddres);
    m_databasePassword = SENSOR_DEFS::getDatabasePassword(newAddres);
    m_connectedToMAC = SENSOR_DEFS::getConnectedToMAC(newAddres);
    m_room.loadFromEEPROM(SENSOR_DEFS::GET_ADDRESS_OF_ROOM(newAddres));
    m_sensorState = static_cast<State>(SENSOR_DEFS::getState(newAddres));

    m_deviceMAC = WiFi.macAddress().c_str();
}

void SensorBase::print() const
{
    Serial.printf("------------------ %s ------------------\n", m_deviceMAC.c_str());
    Serial.printf("| Username\t\t\t|\t%s\n", m_username.c_str());
    Serial.printf("| Password\t\t\t|\t%s\n", m_password.c_str());
    Serial.printf("| SSID\t\t\t\t|\t%s\n", m_ssid.c_str());
    Serial.printf("| WLAN-Password\t\t\t|\t%s\n", m_wlanPassword.c_str());
    Serial.printf("| Database ip\t\t\t|\t%s\n", m_databaseIP.toString().c_str());
    Serial.printf("| Database username\t\t|\t%s\n", m_databaseUsername.c_str());
    Serial.printf("| Database password\t\t|\t%s\n", m_databasePassword.c_str());
    Serial.printf("| Connected to MAC\t\t|\t%s\n", m_connectedToMAC.c_str());
    Serial.printf("| Sensor state\t\t\t|\t%d\n", m_sensorState);
    Serial.print("-----------------------------------------\n");
    m_room.print();
}

void SensorBase::printFromEEPROM() const
{
    Serial.printf("------------------ %s ------------------\n", WiFi.macAddress().c_str());
    Serial.printf("| Username\t\t\t|\t%s\n", SENSOR_DEFS::getUsername().c_str());
    Serial.printf("| Password\t\t\t|\t%s\n", SENSOR_DEFS::getPassword().c_str());
    Serial.printf("| SSID\t\t\t\t|\t%s\n", SENSOR_DEFS::getWifiName().c_str());
    Serial.printf("| WLAN-Password\t\t\t|\t%s\n", SENSOR_DEFS::getWifiPassword().c_str());
    Serial.printf("| Database ip\t\t\t|\t%s\n", SENSOR_DEFS::getDatabaseIP().toString().c_str());
    Serial.printf("| Database username\t\t|\t%s\n", SENSOR_DEFS::getDatabaseUsername().c_str());
    Serial.printf("| Database password\t\t|\t%s\n", SENSOR_DEFS::getDatabasePassword().c_str());
    Serial.printf("| Connected to MAC\t\t|\t%s\n", SENSOR_DEFS::getConnectedToMAC().c_str());
    Serial.printf("| Sensor state\t\t\t|\t%d\n", SENSOR_DEFS::getState());
    Serial.print("-----------------------------------------\n");
    m_room.print();
}

void SensorBase::toDefault(){
    m_deviceMAC = "";
    m_connectedToMAC = "";
    m_sensorState = State::SETUP;
    m_room = Room();
    m_description = "Sensor";
    m_username = "admin";
    m_password = "012345678";
    m_ssid = "";
    m_wlanPassword = ""; // 192, 168, 178, 82
    m_databaseIP = IPAddress(192, 168, 178, 82);
    m_databaseUsername = "esp32User";
    m_databasePassword = "Qwertz1998"; // jGXjVuVhY4vHOc1r
}

String processLogin(const String &var)
{
    Serial.printf("\t |-> Login <-> %s", var.c_str());
    return "";
}

String processConfig(const String &var)
{
    Serial.printf("\t |-> Config <-> %s", var.c_str());
    return "";
}

String processLoginPost(const String &var){
    Serial.printf("\t |-> Login-Post-Request <-> %s", var.c_str());
    return "";
}

void requestRoomDataFromDatabase(MySQL_Connection *conn, Room *room)
{
    Serial.println("> Serialize room data!");

    IPAddress databaseIP = SENSOR_DEFS::getDatabaseIP();

    if (!conn->connected())
        connectToDatabase(
            *conn, 
            databaseIP,
            SENSOR_DEFS::getDatabaseUsername().c_str(),
            SENSOR_DEFS::getDatabasePassword().c_str()
        );

    char bufferCondition[64] = {};

    DATABASE_GENERATE_CONDITION(
        "%s = %d", 
        "ID", room->getID(),
        bufferCondition, 
        64
    );

    vector<string> selectResponse = getDatabaseSelectResponse(
        conn,
        "es_datenbank.raeume",
        "RaumGroesse, MaxPersonenAnzahl, PersonenAnzahl",
        bufferCondition
    );

    if(selectResponse.size() == 3)
    {
        room->setSize(atoi(selectResponse.at(0).c_str()));
        room->setMaxPersonCount(atoi(selectResponse.at(1).c_str()));
        room->setPersonCount(atoi(selectResponse.at(2).c_str()));

        room->print();
        room->saveInEEPROM(SENSOR_DEFS::GET_ADDRESS_OF_ROOM());
        Serial.println("\t- Room serielizetion finished!");
    }else
    {
        Serial.printf("\t- The room with the room ID %d does not exist!\n", room->getID());
    }
}