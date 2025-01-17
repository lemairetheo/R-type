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
    struct User {
        int id;
        std::string username;
        std::string last_connection;
        int total_games_played;
        int total_playtime;
        std::string created_at;
    };

    class UserRepository {
    public:
        explicit UserRepository(DatabaseManager& dbManager) : db(dbManager) {}

        std::optional<User> createUser(const std::string& username);
        std::optional<User> getUser(const std::string& username);
        void updateUserStats(const std::string& username, int playtime);
        void updateLastConnection(const std::string& username);
        std::vector<User> getTopPlayers(int limit = 10);
    private:
        DatabaseManager& db;
    };

}

#endif //USERREPOSITORY_HPP
