#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define DATABASE_SELECT(TABLE, COLUMNS, CONDITION, BUFFER, SIZE)                   \
    {                                                                              \
        BUFFER_TO_DEFAULT_VALUE(BUFFER, SIZE)                                      \
        sprintf(BUFFER, "SELECT %s FROM %s WHERE %s;", COLUMNS, TABLE, CONDITION); \
    }
#define DATABASE_SELECT_ADDITIONAL(TABLE, COLUMNS, CONDITION, BUFFER, ADDITIONAL, SIZE)           \
    {                                                                                             \
        BUFFER_TO_DEFAULT_VALUE(BUFFER, SIZE)                                                     \
        sprintf(BUFFER, "SELECT %s FROM %s WHERE %s %s;", COLUMNS, TABLE, CONDITION, ADDITIONAL); \
    }

#define DATABASE_DELETE(TABLE, CONDITION, BUFFER, SIZE)                \
    {                                                                  \
        BUFFER_TO_DEFAULT_VALUE(BUFFER, SIZE)                          \
        sprintf(BUFFER, "DELETE FROM %s WHERE %s;", TABLE, CONDITION); \
    }

#define DATABASE_UPDATE(TABLE, VALUES, CONDITION, BUFFER, SIZE)                 \
    {                                                                           \
        BUFFER_TO_DEFAULT_VALUE(BUFFER, SIZE)                                   \
        sprintf(BUFFER, "UPDATE %s SET %s WHERE %s", TABLE, VALUES, CONDITION); \
    }

#define DATABASE_INSERT(TABLE, COLUMNS, VALUES, BUFFER, SIZE)                    \
    {                                                                            \
        BUFFER_TO_DEFAULT_VALUE(BUFFER, SIZE)                                    \
        sprintf(BUFFER, "INSERT INTO %s %s VALUES %s;", TABLE, COLUMNS, VALUES); \
    }

#define DATABASE_GENERATE_CONDITION(CONDITION, LHS, RHS, BUFFER, SIZE) \
    {                                                                  \
        BUFFER_TO_DEFAULT_VALUE(BUFFER, SIZE)                          \
        sprintf(BUFFER, CONDITION, LHS, RHS);                          \
    }

#define BUFFER_TO_DEFAULT_VALUE(BUFFER, SIZE) \
    {                                         \
        for (size_t i = 0; i < SIZE; i++)     \
        {                                     \
            BUFFER[i] = 0;                    \
        }                                     \
    }

inline void printDatabaseCommand(const char* buffer, int line, const char* file)
{
    #ifdef DEBUG_MODE
        Serial.printf("[S]> %s\n\t-> File: %s\n\t-> Line: %d\n", buffer, file, line);
    #else
        Serial.printf("[S]> %s\n", buffer);
    #endif
}

/**
 * @brief printRecievedSQLData prints the response to a SELECT request from the database.
 * 
 * @param conn is the database connection handler.
 * @param buffer contains the SQL SELECT command. 
 */
inline void printRecievedSQLData(MySQL_Connection* conn, char* buffer){
    MySQL_Cursor* cursor = new MySQL_Cursor(conn);

    printDatabaseCommand(buffer, __LINE__, __FILE__);
    cursor->execute(buffer);

    column_names *columns = cursor->get_columns();

    // Go through the columns and print them to the console
    for (size_t i = 0; i < columns->num_fields; i++)
    {
        Serial.print(columns->fields[i]->name);
        if(i < columns->num_fields - 1){
            Serial.print(", ");
        }
    }
    Serial.println();

    // Go through the rows and print them to the console
    row_values *rows = nullptr;
    do
    {
        rows = cursor->get_next_row();
        if(rows != nullptr){
            for (size_t i = 0; i < columns->num_fields; i++)
            {
                Serial.print(rows->values[i]);
                if (i < columns->num_fields - 1)
                {
                    Serial.print(", ");
                }
            }
            Serial.println();
        }
    } while (rows != nullptr);

    cursor->close();
    delete cursor;
}

inline std::vector<std::string> getDatabaseSelectResponse(MySQL_Connection* conn, std::string table, std::string colums, std::string condition)
{
    MySQL_Cursor cursor = MySQL_Cursor(conn);

    char buffer[256];
    DATABASE_SELECT(table.c_str(), colums.c_str(), condition.c_str(), buffer, 256);

    printDatabaseCommand(buffer, __LINE__, __FILE__);
    cursor.execute(buffer);

    column_names* column = cursor.get_columns();

    row_values* row = nullptr;
    std::vector<std::string> selectionResult;
    do
    {
        row = cursor.get_next_row();
        if(row != nullptr){
            for (size_t i = 0; i < column->num_fields; i++)
            {
                selectionResult.push_back(row->values[i]);
            }
        }
    } while (row != nullptr);
    
    return selectionResult;
}

inline std::vector<std::string> getDatabaseSelectResponse(MySQL_Connection *conn, std::string table, std::string colums, std::string condition, std::string additional)
{
    MySQL_Cursor cursor = MySQL_Cursor(conn);

    char buffer[256];
    DATABASE_SELECT_ADDITIONAL(table.c_str(), colums.c_str(), condition.c_str(), buffer, additional.c_str(), 256);

    printDatabaseCommand(buffer, __LINE__, __FILE__);
    cursor.execute(buffer);

    column_names *column = cursor.get_columns();

    row_values *row = nullptr;
    std::vector<std::string> selectionResult;
    do
    {
        row = cursor.get_next_row();
        if (row != nullptr)
        {
            for (size_t i = 0; i < column->num_fields; i++)
            {
                selectionResult.push_back(row->values[i]);
            }
        }
    } while (row != nullptr);

    return selectionResult;
}

inline void connectToDatabase(MySQL_Connection& conn, IPAddress& ip, const char* username, const char* password){
    // Establish the database connection
    std::string database_name = "es_datenbank";

    Serial.printf("\n> Try to connect with:\n\t-> IP: %s\n\t-> Username: %s\n\t-> Password: %s\n", ip.toString().c_str(), username, password);

    while (!conn.connect(ip, 3306, const_cast<char *>(username), const_cast<char *>(password), const_cast<char *>(database_name.c_str())))
    {
        delay(1000);
    }
    Serial.println("> Connection to database established!");
}