//
// Created by Jean-Baptiste  Azan on 17/01/2025.
//

#include "UserRepository.hpp"

#include <iostream>
#include <ostream>

namespace rtype::database {
    void UserRepository::updateUserStats(const std::string& username, int playtime) {
    std::string sql = R"(
        UPDATE users
        SET total_games_played = total_games_played + 1,
            total_playtime = total_playtime + ?2
        WHERE username = ?1;
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, playtime);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to update user stats");
    }
    }

    std::optional<User> UserRepository::getUser(const std::string& username) {
        std::string sql = "SELECT * FROM users WHERE username = ?1;";
        std::optional<User> result;

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = User{
                sqlite3_column_int(stmt, 0),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
                sqlite3_column_int(stmt, 3),
                sqlite3_column_int(stmt, 4),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))
            };
        }

        sqlite3_finalize(stmt);
        return result;
    }

    std::optional<User> UserRepository::createUser(const std::string& username) {
        std::cerr << "Attempting to create user: " << username << std::endl;
        std::string sql = R"(
        INSERT INTO users (username, last_connection, total_games_played, total_playtime, created_at)
        VALUES (?1, CURRENT_TIMESTAMP, 0, 0, CURRENT_TIMESTAMP)
        ON CONFLICT(username) DO NOTHING;
    )";

        sqlite3_stmt* stmt;
        std::cerr << "Preparing statement..." << std::endl;
        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db.getHandle()) << std::endl;
            throw std::runtime_error("Failed to prepare statement");
        }

        std::cerr << "Binding parameters..." << std::endl;
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        std::cerr << "Executing statement..." << std::endl;
        rc = sqlite3_step(stmt);
        std::cerr << "Statement execution result: " << rc << std::endl;
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Failed to create user with error: " << sqlite3_errmsg(db.getHandle()) << std::endl;
            throw std::runtime_error("Failed to create user");
        }

        std::cerr << "User created successfully, retrieving user..." << std::endl;
        return getUser(username);
    }


    void UserRepository::updateLastConnection(const std::string& username) {
        std::string sql = R"(
        UPDATE users
        SET last_connection = CURRENT_TIMESTAMP
        WHERE username = ?1;
    )";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement for updating last connection");
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to update last connection time");
        }
    }

    std::vector<User> UserRepository::getTopPlayers(int limit) {
        std::vector<User> users;
        std::string sql = R"(
        SELECT *
        FROM users
        ORDER BY total_playtime DESC, total_games_played DESC
        LIMIT ?1;
    )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement for getting top players");
        }

        sqlite3_bind_int(stmt, 1, limit);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            users.push_back(User{
                sqlite3_column_int(stmt, 0),                                          // id
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),         // username
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),         // last_connection
                sqlite3_column_int(stmt, 3),                                         // total_games_played
                sqlite3_column_int(stmt, 4),                                         // total_playtime
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))          // created_at
            });
        }

        sqlite3_finalize(stmt);
        return users;
    }
}
