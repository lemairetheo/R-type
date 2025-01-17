/**
* @file packetType.hpp
 * @brief Définit les types de paquets et structures pour la communication réseau
 * @author Votre Nom
 * @date 2024
 */

#pragma once
#include <cstdint>

namespace rtype::network {

    struct ConnectRequestPacket {
        char username[32];
    };
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
     * @brief Types de paquets supportés
     *
     * Énumération de tous les types de paquets possibles dans le protocole.
     */
    enum class PacketType : uint8_t {
        CONNECT_REQUEST = 0x01,    ///< Demande de connexion client
        CONNECT_RESPONSE = 0x02,   ///< Réponse du serveur à une demande de connexion
        DISCONNECT = 0x03,         ///< Notification de déconnexion
        HEARTBEAT = 0x04,         ///< Paquet de maintien de connexion

        PLAYER_INPUT = 0x10,      ///< Entrées du joueur
        PLAYER_SHOOT = 0x12,      ///< Missile du joueur
        GAME_STATE = 0x11,         ///< État du jeu
        ENTITY_UPDATE = 0x20, ///< Mise a jour d'une entité
        ENTITY_DEATH = 0x21, //< Information sur une entité morte
        END_GAME_STATE = 0x22,
    };

    struct EntityUpdatePacket {
        uint32_t entityId;   ///< ID de l'entité
        uint32_t entityId2;
        int type;            ///< le nombre signifie le type de l'entité
        float x;             ///< Position X
        float y;             ///< Position Y
        float dx;            ///< Vélocité X
        float dy;            ///< Vélocité Y
        int life;   ///< vie du player
        int score;  ///< score de la partie
        int level; ///< level actuel
    };


    struct PlayerInputPacket {
        bool up;
        bool down;
        bool left;
        bool right;
        bool space;
        bool ultimate;
    };

#pragma pack(pop)
} // namespace rtype::network