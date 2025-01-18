/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ScoreRepository
*/

#pragma once
#include "DatabaseManager.hpp"
#include <optional>
#include <vector>

namespace rtype::database {
    struct PlayerScore {
        int id;
        std::string username;  // Changé de int à string
        int score_time;
        int level_reached;
        int enemies_killed;
        std::string game_date;
    };

    class ScoreRepository {
    public:
        explicit ScoreRepository(DatabaseManager& dbManager) : db(dbManager) {}

        void updatePlayerScore(const std::string& username, int time, int level, int enemiesKilled);
        std::optional<PlayerScore> getPlayerBestScore(const std::string& username);
        std::vector<PlayerScore> getTopScores(int limit = 10);
        std::vector<PlayerScore> getUserScores(int userId, int limit = 10);

    private:
        DatabaseManager& db;
    };
} // namespace rtype::database
