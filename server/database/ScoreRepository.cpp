/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ScoreRepository
*/

#include "ScoreRepository.hpp"


namespace rtype::database {
    void ScoreRepository::updatePlayerScore(const std::string& username, int time) const {
        std::string sql = R"(
        INSERT INTO player_scores (username, best_time, games_won)
        VALUES (?1, ?2, 1)
        ON CONFLICT(username) DO UPDATE SET
            best_time = MIN(best_time, ?2),
            games_won = games_won + 1,
            last_game_time = CURRENT_TIMESTAMP
        WHERE username = ?1;
    )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, time);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to update score");
        }
    }

    std::optional<PlayerScore> ScoreRepository::getPlayerBestScore(const std::string& username) const {
        std::optional<PlayerScore> result;
        std::string sql = "SELECT * FROM player_scores WHERE username = ?1;";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = PlayerScore{
                sqlite3_column_int(stmt, 0),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                sqlite3_column_int(stmt, 2),
                sqlite3_column_int(stmt, 3),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
            };
        }

        sqlite3_finalize(stmt);
        return result;
    }

    std::vector<PlayerScore> ScoreRepository::getTopScores(int limit) const {
        std::vector<PlayerScore> scores;
        std::string sql = "SELECT * FROM player_scores ORDER BY best_time ASC LIMIT ?1;";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db.getHandle(), sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_int(stmt, 1, limit);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            scores.push_back(PlayerScore{
                sqlite3_column_int(stmt, 0),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                sqlite3_column_int(stmt, 2),
                sqlite3_column_int(stmt, 3),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
            });
        }

        sqlite3_finalize(stmt);
        return scores;
    }
}