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
    /**
     * @struct PlayerScore
     * @brief Represents a player's score.
     */
    struct PlayerScore {
        int id;
        std::string username;
        int score_time;
        int level_reached;
        int enemies_killed;
        std::string game_date;
    };
    /**
     * @class ScoreRepository
     * @brief Repository for score data.
     */
    class ScoreRepository {

    public:
        explicit ScoreRepository(DatabaseManager& dbManager) : db(dbManager) {}
        /**
         * @brief Updates the player's score.
         * @param username The username of the player.
         * @param time The time taken to complete the game.
         * @param level The level reached by the player.
         * @param enemiesKilled The number of enemies killed by the player.
         */
        void updatePlayerScore(const std::string& username, int time, int level, int enemiesKilled);
        /**
         * @brief Gets the player's best score.
         * @param username The username of the player.
         * @return The player's best score, if found.
         */
        std::optional<PlayerScore> getPlayerBestScore(const std::string& username);
        /**
         * @brief Gets the top scores.
         * @param limit The maximum number of scores to get.
         * @return The top scores.
         */
        std::vector<PlayerScore> getTopScores(int limit = 10);
        /**
         * @brief Gets the scores of a user.
         * @param userId The ID of the user.
         * @param limit The maximum number of scores to get.
         * @return The scores of the user.
         */
        std::vector<PlayerScore> getUserScores(int userId, int limit = 10);

    private:
        DatabaseManager& db;
    };
} // namespace rtype::database
