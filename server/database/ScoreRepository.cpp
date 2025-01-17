/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ScoreRepository
*/

#include "ScoreRepository.hpp"

#include <iostream>
#include <ostream>


namespace rtype::database {
    void ScoreRepository::updatePlayerScore(const std::string& username, int time, int level, int enemiesKilled) {
        std::cerr << "Updating score for user " << username << " with time " << time << ", level " << level << ", enemies " << enemiesKilled << std::endl;

        std::string sql = R"(
        INSERT INTO player_scores (username, score_time, level_reached, enemies_killed)
        VALUES (?1, ?2, ?3, ?4);
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
        sqlite3_bind_int(stmt, 2, time);
        sqlite3_bind_int(stmt, 3, level);
        sqlite3_bind_int(stmt, 4, enemiesKilled);

        std::cerr << "Executing statement..." << std::endl;
        rc = sqlite3_step(stmt);
        std::cerr << "Statement execution result: " << rc << std::endl;
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Failed to update score with error: " << sqlite3_errmsg(db.getHandle()) << std::endl;
            throw std::runtime_error("Failed to update score");
        }
        std::cerr << "Score updated successfully" << std::endl;
    }

    std::optional<PlayerScore> ScoreRepository::getPlayerBestScore(const std::string& username) {
        std::string sql = R"(
        SELECT * FROM player_scores
        WHERE username = ?1
        ORDER BY score_time ASC
        LIMIT 1;
    )";

        std::optional<PlayerScore> result;
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = PlayerScore{
                sqlite3_column_int(stmt, 0),         // id
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),  // username
                sqlite3_column_int(stmt, 2),         // score_time
                sqlite3_column_int(stmt, 3),         // level_reached
                sqlite3_column_int(stmt, 4),         // enemies_killed
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))   // game_date
            };
        }

        sqlite3_finalize(stmt);
        return result;
    }
}
