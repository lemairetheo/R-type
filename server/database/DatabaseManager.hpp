/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** DatabaseManager
*/


#pragma once
#include <sqlite3.h>
#include <string>
#include <functional>
#include <stdexcept>

namespace rtype::database {
    /**
     * @class DatabaseManager
     * @brief Manages the SQLite database.
     */
    class DatabaseManager {
        /**
         * @class Statement
         * @brief Represents a prepared statement.
         */
        class Statement {
        public:
            Statement(sqlite3_stmt* stmt) : stmt_(stmt) {}
            ~Statement() { if (stmt_) sqlite3_finalize(stmt_); }

            void bind(int idx, int value) {
                sqlite3_bind_int(stmt_, idx, value);
            }

            void bind(int idx, const std::string& value) {
                sqlite3_bind_text(stmt_, idx, value.c_str(), -1, SQLITE_STATIC);
            }

            int step() {
                return sqlite3_step(stmt_);
            }

            int column_int(int col) {
                return sqlite3_column_int(stmt_, col);
            }

            std::string column_text(int col) {
                const unsigned char* text = sqlite3_column_text(stmt_, col);
                return text ? std::string(reinterpret_cast<const char*>(text)) : std::string();
            }

        private:
            sqlite3_stmt* stmt_;
        };
        /**
         * @brief Prepares a statement.
         * @param sql The SQL query.
         * @return The prepared statement.
         */
        [[nodiscard]] Statement prepare(const std::string& sql) const {
            sqlite3_stmt* stmt;
            int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
            }
            return Statement(stmt);
        }
    public:
        explicit DatabaseManager(const std::string& dbPath = "rtype.db");
        ~DatabaseManager();
        DatabaseManager(const DatabaseManager&) = delete;
        /**
         * @brief Executes an SQL query.
         * @param sql The SQL query.
         */
        DatabaseManager& operator=(const DatabaseManager&) = delete;
        /**
         * @brief Executes an SQL query.
         * @param sql The SQL query.
         */
        void execute(const std::string& sql) const;
        /**
         * @brief Executes an SQL query with a callback.
         * @param sql The SQL query.
         * @param callback The callback to execute.
         */
        void query(const std::string& sql, std::function<void(sqlite3_stmt*)> callback) const;
        /**
         * @brief Initializes the database.
         */
        void initializeDatabase();
        /**
         * @brief Gets the SQLite handle.
         * @return The SQLite handle.
         */
        [[nodiscard]] sqlite3* getHandle() const { return db; }
    private:
        sqlite3* db;
    };
};

