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
     * @brief En-tête commun pour tous les paquets réseau
     *
     * Cette structure est utilisée comme en-tête pour chaque paquet envoyé
     * entre le client et le serveur.
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
        GAME_STATE = 0x11,         ///< État du jeu
        ENTITY_UPDATE = 0x20,
    };

    struct EntityUpdatePacket {
        uint32_t entityId;   ///< ID de l'entité
        float x;             ///< Position X
        float y;             ///< Position Y
        float dx;            ///< Vélocité X
        float dy;            ///< Vélocité Y
    };


    struct PlayerInputPacket {
        bool up;
        bool down;
        bool left;
        bool right;
    };
#pragma pack(pop)
} // namespace rtype::network