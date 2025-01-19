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

namespace rtype::database {
    class DatabaseManager {
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
        DatabaseManager& operator=(const DatabaseManager&) = delete;
        void execute(const std::string& sql) const;
        void query(const std::string& sql, std::function<void(sqlite3_stmt*)> callback) const;
        void initializeDatabase();
        sqlite3* getHandle() { return db; }
    private:
        sqlite3* db;
    };
};

