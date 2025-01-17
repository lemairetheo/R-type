/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** DatabaseManager
*/

#include "DatabaseManager.hpp"

namespace rtype::database {
    DatabaseManager::DatabaseManager(const std::string& dbPath) : db(nullptr) {
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc) {
            std::string error = sqlite3_errmsg(db);
            sqlite3_close(db);
            throw std::runtime_error("Can't open database: " + error);
        }
        initializeDatabase();
    }

    DatabaseManager::~DatabaseManager() {
        if (db)
            sqlite3_close(db);
    }

    void DatabaseManager::execute(const std::string& sql) const {
        char* errorMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
        if (rc != SQLITE_OK) {
            std::string error = errorMsg;
            sqlite3_free(errorMsg);
            throw std::runtime_error("SQL error: " + error);
        }
    }

    void DatabaseManager::query(const std::string& sql, std::function<void(sqlite3_stmt*)> callback) const {
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            callback(stmt);
        }

        sqlite3_finalize(stmt);
    }

    void DatabaseManager::initializeDatabase() {
        const char* sql = R"(
        CREATE TABLE IF NOT EXISTS player_scores (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            best_time INTEGER NOT NULL,
            games_won INTEGER DEFAULT 0,
            last_game_time DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";
        execute(sql);
    }
}