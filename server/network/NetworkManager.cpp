#include "NetworkManager.hpp"

#include "database/ScoreRepository.hpp"

namespace rtype::network {

    NetworkManager::NetworkManager(uint16_t port)
        : ANetwork(port)
        , socket(io_context)
        , running(false)
        , receive_buffer(1024) {
    }

    NetworkManager::~NetworkManager() {
        stop();
    }

    void NetworkManager::start() {
        if (running) return;
        try {
            socket.open(asio::ip::udp::v4());
            socket.bind(asio::ip::udp::endpoint(asio::ip::address_v4::any(), port));
            running = true;
            startReceive();
            io_thread = std::thread([this]() {
                try {
                    io_context.run();
                } catch (const std::exception& e) {
                    std::cout << "Network error: " << e.what() << std::endl;
                }
            });
            std::cout << "Network Manager started on port " << port << std::endl;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to start network: " + std::string(e.what()));
        }
    }

    std::vector<uint8_t> NetworkManager::createScoreUpdatePacket(const std::string& username, int time, int score) {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(ScoreUpdatePacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* scoreUpdate = reinterpret_cast<ScoreUpdatePacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::SCORE_UPDATE);
        header->length = packet.size();
        header->sequence = 0;

        std::strncpy(scoreUpdate->username, username.c_str(), sizeof(scoreUpdate->username) - 1);
        scoreUpdate->username[sizeof(scoreUpdate->username) - 1] = '\0';
        scoreUpdate->time = time;
        scoreUpdate->score = score;

        return packet;
    }

    std::vector<uint8_t> NetworkManager::createBestScorePacket(const std::string& username, int bestTime, int gamesWon) {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(BestScorePacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* bestScore = reinterpret_cast<BestScorePacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::BEST_SCORE);
        header->length = packet.size();
        header->sequence = 0;

        std::strncpy(bestScore->username, username.c_str(), sizeof(bestScore->username) - 1);
        bestScore->username[sizeof(bestScore->username) - 1] = '\0';
        bestScore->best_time = bestTime;
        bestScore->games_won = gamesWon;

        return packet;
    }

    const asio::ip::udp::endpoint& NetworkManager::getClientEndpoint(const std::string& clientId) const {
        auto it = clients.find(clientId);
        if (it == clients.end()) {
            throw std::runtime_error("Client not found: " + clientId);
        }
        return it->second;
    }

    std::vector<uint8_t> NetworkManager::createEntityUpdatePacket(EntityID entityId, int type, const Position& pos, const Velocity& vel,
    int life, int score, int level) {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(EntityUpdatePacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* update = reinterpret_cast<EntityUpdatePacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::ENTITY_UPDATE);
        header->length = packet.size();
        header->sequence = 0;

        update->entityId = entityId;
        update->type = type;
        update->x = pos.x;
        update->y = pos.y;
        update->dx = vel.dx;
        update->dy = vel.dy;
        update->life = life;
        update->score = score;
        update->level = level;
        return packet;
    }

    std::vector<uint8_t> NetworkManager::createEntityDeathPacket(EntityID entity, EntityID missile) {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(EntityUpdatePacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* update = reinterpret_cast<EntityUpdatePacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::ENTITY_DEATH);
        header->length = packet.size();
        header->sequence = 0;

        update->entityId = entity;
        update->entityId2 = missile;
        update->type = 0;

        return packet;
    }

    std::vector<uint8_t> NetworkManager::createEndGamePacket() {
        std::vector<uint8_t> packet(sizeof(PacketHeader));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::END_GAME_STATE);
        header->length = packet.size();
        header->sequence = 0;

        return packet;
    }

    std::vector<uint8_t> NetworkManager::createLooseGamePacket() {
        std::vector<uint8_t> packet(sizeof(PacketHeader));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::LOOSE_GAME_STATE);
        header->length = packet.size();
        header->sequence = 0;

        return packet;
    }

    void NetworkManager::stop() {
        if (!running) return;

        running = false;
        io_context.stop();

        if (socket.is_open()) {
            socket.close();
        }

        if (io_thread.joinable()) {
            io_thread.join();
        }

        std::cout << "Network Manager stopped" << std::endl;
    }

    void NetworkManager::setMessageCallback(
        std::function<void(const std::vector<uint8_t>&, const asio::ip::udp::endpoint&)> callback) {
        messageCallback = std::move(callback);
    }

    void NetworkManager::broadcast(const std::vector<uint8_t>& data) {
        for (const auto& [id, client] : clients) {
            sendTo(data, client);
        }
    }

    std::vector<uint8_t> NetworkManager::createPlayerStatsPacket(const database::User& user)
    {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(PlayerStatsPacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* stats = reinterpret_cast<PlayerStatsPacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::PLAYER_STATS);
        header->length = packet.size();
        header->sequence = 0;

        std::strncpy(stats->username, user.username.c_str(), sizeof(stats->username) - 1);
        stats->username[sizeof(stats->username) - 1] = '\0';
        stats->total_games_played = user.total_games_played;
        stats->total_playtime = user.total_playtime;
        // Les autres champs devront être ajoutés à la structure User ou récupérés d'une autre manière

        return packet;
    }

    std::vector<uint8_t> NetworkManager::createBestScorePacket(
    const std::string& username, int bestTime, int gamesWon, int totalPlaytime, float avgScore)
    {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(BestScorePacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* bestScore = reinterpret_cast<BestScorePacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::BEST_SCORE);
        header->length = packet.size();
        header->sequence = 0;

        std::strncpy(bestScore->username, username.c_str(), sizeof(bestScore->username) - 1);
        bestScore->username[sizeof(bestScore->username) - 1] = '\0';
        bestScore->best_time = bestTime;
        bestScore->games_won = gamesWon;
        bestScore->total_playtime = totalPlaytime;
        bestScore->avg_score = avgScore;

        return packet;
    }

    std::vector<uint8_t> NetworkManager::createGameStatsPacket(
    int level, int enemiesKilled, int score, int timeElapsed, int lifeRemaining)
    {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(GameStatsPacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* stats = reinterpret_cast<GameStatsPacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::GAME_STATS);
        header->length = packet.size();
        header->sequence = 0;

        stats->current_level = level;
        stats->enemies_killed = enemiesKilled;
        stats->current_score = score;
        stats->time_elapsed = timeElapsed;
        stats->life_remaining = lifeRemaining;

        return packet;
    }

    std::vector<uint8_t> NetworkManager::createScoreUpdatePacket(
    const std::string& username, int time, int score, int levelReached, int enemiesKilled)
    {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(ScoreUpdatePacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* scoreUpdate = reinterpret_cast<ScoreUpdatePacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::SCORE_UPDATE);
        header->length = packet.size();
        header->sequence = 0;

        std::strncpy(scoreUpdate->username, username.c_str(), sizeof(scoreUpdate->username) - 1);
        scoreUpdate->username[sizeof(scoreUpdate->username) - 1] = '\0';
        scoreUpdate->time = time;
        scoreUpdate->score = score;
        scoreUpdate->level_reached = levelReached;
        scoreUpdate->enemies_killed = enemiesKilled;

        return packet;
    }

    std::vector<uint8_t> NetworkManager::createLeaderboardPacket(const std::vector<database::PlayerScore>& scores) {
        std::vector<uint8_t> packet(sizeof(PacketHeader) + sizeof(LeaderboardPacket));
        auto* header = reinterpret_cast<PacketHeader*>(packet.data());
        auto* leaderboard = reinterpret_cast<LeaderboardPacket*>(packet.data() + sizeof(PacketHeader));

        header->magic[0] = 'R';
        header->magic[1] = 'T';
        header->version = 1;
        header->type = static_cast<uint8_t>(PacketType::LEADERBOARD_RESPONSE);
        header->length = packet.size();
        header->sequence = 0;

        leaderboard->nb_entries = std::min(scores.size(), size_t(10));
        for (size_t i = 0; i < leaderboard->nb_entries; ++i) {
            std::strncpy(leaderboard->entries[i].username, scores[i].username.c_str(), 31);
            leaderboard->entries[i].username[31] = '\0';
            leaderboard->entries[i].score = scores[i].enemies_killed;  // Utiliser enemies_killed comme score
            leaderboard->entries[i].level_reached = scores[i].level_reached;
            leaderboard->entries[i].time = scores[i].score_time;
        }

        return packet;
    }

    void NetworkManager::sendTo(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& client) {
        socket.async_send_to(
            asio::buffer(data),
            client,
            [](const asio::error_code& error, std::size_t /*bytes_transferred*/) {
                if (error) {
                    std::cout << "Send error: " << error.message() << std::endl;
                }
            }
        );
    }

    void NetworkManager::startReceive() {
        socket.async_receive_from(
            asio::buffer(receive_buffer),
            sender_endpoint,
            [this](const asio::error_code& error, std::size_t bytes_transferred) {
                this->handleReceive(error, bytes_transferred);
            }
        );
    }

    void NetworkManager::handleReceive(const asio::error_code& error, std::size_t bytes_transferred) {
        if (!error && bytes_transferred > 0) {
            const auto* header = reinterpret_cast<const PacketHeader*>(receive_buffer.data());
            std::string clientId = sender_endpoint.address().to_string() + ":" +
                                 std::to_string(sender_endpoint.port());

            if (header->type == static_cast<uint8_t>(PacketType::CONNECT_REQUEST)) {
                if (clients.find(clientId) == clients.end()) {
                    clients[clientId] = sender_endpoint;
                    std::cout << "New client connected: " << clientId << std::endl;
                }
            }

            if (messageCallback) {
                std::vector<uint8_t> received_data(
                    receive_buffer.begin(),
                    receive_buffer.begin() + bytes_transferred
                );
                messageCallback(received_data, sender_endpoint);
            }

            if (running) {
                startReceive();
            }
        } else if (error != asio::error::operation_aborted && running) {
            std::cout << "Receive error: " << error.message() << std::endl;
            startReceive();
        }
    }

    void NetworkManager::update() {
        checkTimeouts();
    }

    void NetworkManager::updateClientActivity(const std::string& clientId) {
        clientLastSeen[clientId] = std::chrono::steady_clock::now();
    }

    void NetworkManager::checkTimeouts() {
        auto now = std::chrono::steady_clock::now();
        std::vector<std::string> disconnectedClients;

        for (const auto& [clientId, lastSeen] : clientLastSeen) {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastSeen).count() > 5) {
                disconnectedClients.push_back(clientId);
            }
        }

        for (const auto& clientId : disconnectedClients) {
            handleClientDisconnection(clientId);
        }
    }

    void NetworkManager::handleClientDisconnection(const std::string& clientId) {
        if (auto it = clients.find(clientId); it != clients.end()) {
            clients.erase(it);
            clientLastSeen.erase(clientId);
            std::vector<uint8_t> packet(sizeof(PacketHeader));
            auto* header = reinterpret_cast<PacketHeader*>(packet.data());
            header->magic[0] = 'R';
            header->magic[1] = 'T';
            header->version = 1;
            header->type = static_cast<uint8_t>(PacketType::DISCONNECT);
            header->length = packet.size();

            broadcast(packet);
        }
    }
}
