/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** DatabaseManager
*/

#include "DatabaseManager.hpp"

#include <iostream>

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
        std::cerr << "Executing SQL: " << sql << std::endl;  // Log SQL
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
        if (rc != SQLITE_OK) {
            std::string error = errorMsg;
            std::cerr << "SQL Error Code: " << rc << std::endl;       // Log error code
            std::cerr << "SQL Error Message: " << error << std::endl; // Log error message
            sqlite3_free(errorMsg);
            throw std::runtime_error("SQL error: " + error);
        }
        std::cerr << "SQL executed successfully" << std::endl;  // Log success
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
        std::cerr << "Starting database initialization..." << std::endl;
        const char* sql = R"(
        BEGIN TRANSACTION;

        DROP TABLE IF EXISTS player_scores;
        DROP TABLE IF EXISTS users;

        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            last_connection DATETIME DEFAULT CURRENT_TIMESTAMP,
            total_games_played INTEGER DEFAULT 0,
            total_playtime INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS player_scores (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            score_time INTEGER NOT NULL,         -- Temps
            level_reached INTEGER NOT NULL,      -- Niveau atteint
            enemies_killed INTEGER DEFAULT 0,    -- Ennemis tués
            game_date DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(username) REFERENCES users(username)
        );

        CREATE INDEX IF NOT EXISTS idx_scores_username ON player_scores(username);
        CREATE INDEX IF NOT EXISTS idx_username ON users(username);

        COMMIT;
    )";
        std::cerr << "Executing SQL schema..." << std::endl;
        execute(sql);
        std::cerr << "Database initialization completed successfully" << std::endl;
    }
}
