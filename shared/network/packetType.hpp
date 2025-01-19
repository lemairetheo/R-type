/**
* @file packetType.hpp
 * @brief Définit les types de paquets et structures pour la communication réseau
 * @author Votre Nom
 * @date 2024
 */

#pragma once
#include <cstdint>

namespace rtype::network {
    /**
     * @brief connect request packet
     *
     * This structure defines the connect request packet in the R-Type protocol.
     */
    struct ConnectRequestPacket {
        char username[32];
    };
    /**
     * @brief connect response packet
     *
     * This structure defines the connect response packet in the R-Type protocol.
     */
    struct ConnectResponsePacket {
        uint32_t playerId;    ///< ID unique du joueur
        bool success;         ///< Si la connexion est acceptée
        uint32_t entityId;    ///< ID de l'entité associée au joueur
    };
    /**
     * @brief packet header
     * This structure defines the header of a packet in the R-Type protocol.
     */
    #pragma pack(push, 1)
        struct PacketHeader {
            uint8_t magic[2];      ///< Magic number pour identifier les paquets R-Type (RT)
            uint8_t version;       ///< Version du protocole
            uint8_t type;         ///< Type de paquet (@see PacketType)
            uint16_t length;      ///< Longueur totale du paquet incluant l'en-tête
            uint16_t sequence;    ///< Numéro de séquence pour le suivi des paquets
        };

    /**
     * @brief packet types enumeration
     *
     * This enumeration defines the different types of packets in the R-Type protocol.
     */
    enum class PacketType : uint8_t {
        CONNECT_REQUEST = 0x01,    ///< ask for a connection
        CONNECT_RESPONSE = 0x02,   ///< response to a connection request
        DISCONNECT = 0x03,         ///< ask for a disconnection
        HEARTBEAT = 0x04,         ///< keep the connection alive
        PLAYER_INPUT = 0x10,      ///< player input
        PLAYER_SHOOT = 0x12,      ///< player shoot
        GAME_STATE = 0x11,         ///< game state
        ENTITY_UPDATE = 0x20, ///< Update of an entity
        ENTITY_DEATH = 0x21, ///< Entity death
        END_GAME_STATE = 0x22, ///< End of the game
        LOOSE_GAME_STATE = 0x23, ///< Game loose
        SCORE_UPDATE = 0x30,    ///< score as been update
        BEST_SCORE = 0x31,      ///< Best score of the player as been set
        PLAYER_STATS = 0x32,    ///< Player statistics
        GAME_STATS = 0x33,       ///< Game statistics
        LEADERBOARD_REQUEST = 0x40, ///< Leaderboard request
        LEADERBOARD_RESPONSE = 0x41 ///< Leaderboard response
    };

    /**
     * @brief player input packet
     *
     * This structure defines the player input packet in the R-Type protocol.
     */
    struct EntityUpdatePacket {
        uint32_t entityId;   ///< ID of the entity
        uint32_t entityId2;
        int type;            ///< the type of the entity
        float x;             ///< Position X
        float y;             ///< Position Y
        float dx;            ///< Velocity X
        float dy;            ///< Velocity Y
        int life;   ///< life of the entity
        int score;  ///< score of the entity
        int level; ///< current level of the entity
    };

    /**
     * @brief player input packet
     *
     * This structure defines the player input packet in the R-Type protocol.
     */
    struct PlayerInputPacket {
        bool up;
        bool down;
        bool left;
        bool right;
        bool space;
        bool ultimate;
    };
    /**
     * @brief player shoot packet
     *
     * This structure defines the player shoot packet in the R-Type protocol.
     */
    struct ScoreUpdatePacket {
        char username[32];
        int32_t time;
        int32_t score;
        int32_t level_reached;
        int32_t enemies_killed;
    };
    /**
     * @brief player shoot packet
     *
     * This structure defines the player shoot packet in the R-Type protocol.
     */
    struct BestScorePacket {
        char username[32];
        int32_t best_time;
        int32_t games_won;
        int32_t total_playtime;
        float avg_score;
    };
    /**
     * @brief player's statistics packet
     *
     * This structure defines the player's statistics packet in the R-Type protocol.
     */
    struct PlayerStatsPacket {
        char username[32];
        int32_t total_games_played;
        int32_t total_playtime;
        int32_t best_level_reached;
        int32_t total_enemies_killed;
        int32_t highest_score;
    };
    /**
     * @brief game's statistics packet
     *
     * This structure defines the game's statistics packet in the R-Type protocol.
     */
    struct GameStatsPacket {
        int32_t current_level;
        int32_t enemies_killed;
        int32_t current_score;
        int32_t time_elapsed;
        int32_t life_remaining;
    };
    /**
     * @brief leaderboard request packet
     *
     * This structure defines the leaderboard request packet in the R-Type protocol.
     */
    struct LeaderboardEntry {
        char username[32];
        int32_t score;
        int32_t level_reached;
        int32_t time;
    };
    /**
     * @brief leaderboard request packet
     *
     * This structure defines the leaderboard request packet in the R-Type protocol.
     */
    struct LeaderboardPacket {
        uint32_t nb_entries;
        LeaderboardEntry entries[10];
    };

#pragma pack(pop)
} // namespace rtype::network