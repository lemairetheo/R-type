/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** DatabaseManager
*/


#pragma once
#include <sqlite3.h>
#include <string>
#include <functional>

namespace rtype::database {
    class DatabaseManager {
    public:
        explicit DatabaseManager(const std::string& dbPath = "rtype.db");
        ~DatabaseManager();
        DatabaseManager(const DatabaseManager&) = delete;
        DatabaseManager& operator=(const DatabaseManager&) = delete;
        void execute(const std::string& sql) const;
        void query(const std::string& sql, std::function<void(sqlite3_stmt*)> callback) const;
        void initializeDatabase();
        sqlite3* getHandle() { return db; }
    private:
        sqlite3* db;
    };
};

