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
        std::string username;
        int best_time;
        int games_won;
        std::string last_game_time;
    };
    class ScoreRepository {
        public:
            explicit ScoreRepository(DatabaseManager& dbManager);
            void updatePlayerScore(const std::string& username, int time) const;
            std::optional<PlayerScore> getPlayerBestScore(const std::string& username) const;
            std::vector<PlayerScore> getTopScores(int limit = 10) const;
        private:
            DatabaseManager& db;
    };
} // namespace rtype::database
