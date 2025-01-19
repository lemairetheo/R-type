//
// Created by Jean-Baptiste  Azan on 17/01/2025.
//

#ifndef USERREPOSITORY_HPP
#define USERREPOSITORY_HPP

#include <string>
#include <optional>
#include <vector>
#include "DatabaseManager.hpp"




namespace rtype::database {
    /**
     * @struct User
     * @brief Represents a user in the database.
     */
    struct User {
        int id;
        std::string username;
        std::string last_connection;
        int total_games_played;
        int total_playtime;
        std::string created_at;
    };
    /**
     * @class UserRepository
     * @brief Repository for user data.
     */
    class UserRepository {
    public:
        explicit UserRepository(DatabaseManager& dbManager) : db(dbManager) {}
        /**
         * @brief Creates a new user.
         * @param username The username of the user.
         * @return The created user, if successful.
         */
        std::optional<User> createUser(const std::string& username);
        /**
         * @brief Gets a user by their username.
         * @param username The username of the user.
         * @return The user, if found.
         */
        std::optional<User> getUser(const std::string& username);
        /**
         * @brief Updates the user's stats.
         * @param username The username of the user.
         * @param playtime The playtime to add.
         */
        void updateUserStats(const std::string& username, int playtime);
        /**
         * @brief Updates the user's last connection time.
         * @param username The username of the user.
         */
        void updateLastConnection(const std::string& username);
        /**
         * @brief Gets the top players.
         * @param limit The maximum number of players to get.
         * @return The top players.
         */
        std::vector<User> getTopPlayers(int limit = 10);
    private:
        /**
         * @brief The database manager instance
         */
        DatabaseManager& db;
    };

}

#endif //USERREPOSITORY_HPP
