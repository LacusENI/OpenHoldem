#include <algorithm>
#include <cassert>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "ClientController.h"
#include "ClientHandler.h"
#include "ClientModel.h"
#include "Deck.hpp"
#include "Event.hpp"
#include "GameTypes.hpp"
#include "LocalLink.h"
#include "NullClientView.h"
#include "Player.hpp"
#include "ServerGateway.h"
#include "ServerHandler.h"
#include "ServerSession.h"

using namespace poker;

namespace {
/** 简单机器人：轮到自己座位时 check/call（短筹码即 all-in 跟注）。 */
class AutoClient final : public NullClient {
public:
    void setGateway(ServerGateway& gateway) { this->gateway = &gateway; }

    void on(const GameEvent& event) override {
        if (const auto* joined = std::get_if<Event::JoinResponse>(&event)) {
            seat = joined->seat;
            return;
        }
        if (const auto* turn = std::get_if<Event::TurnStarted>(&event)) {
            if (turn->player != seat) return;
            assert(gateway != nullptr && "AutoClient: gateway 未设置");
            const chips_t target_bet =
                (turn->round_bet <= turn->player_bet)
                    ? turn->player_bet
                    : std::min(turn->round_bet, turn->player_bet + turn->player_chips);
            gateway->send(Command::SubmitAction{.target_bet = target_bet, .is_fold = false});
        }
    }

private:
    ServerGateway* gateway = nullptr;
    seat_t seat = INVALID_SEAT;
};

/** 本地 bot 的直连网关：同步投递命令，并补上自己的 ClientSession。 */
class DirectServerGateway final : public ServerGateway {
public:
    DirectServerGateway(ServerHandler& server, client_id_t id, ClientHandler& handler)
        : server(server), id(id), handler(handler) {}

    void send(const GameCommand& command) override {
        server.on(command, ClientSession{id, handler});
    }

private:
    ServerHandler& server;
    client_id_t id;
    ClientHandler& handler;
};
} // namespace

int main(int argc, char** argv) {
    std::uint32_t seed = Deck::randomSeed();
    if (argc > 1) seed = static_cast<std::uint32_t>(std::stoul(argv[1]));
    Deck deck(seed);
    std::cout << std::format("[Loopback] deck seed = {}\n", deck.getSeed());

    ServerSession session(deck);
    ClientModel model;
    NullClientView view;

    LocalLink link(/*client_id=*/1);
    ClientController client(link.clientSide(), model, view);
    link.attach(session.controller, client);

    constexpr int kPlayers = 3;
    std::vector<std::unique_ptr<AutoClient>> bots;
    std::vector<std::unique_ptr<DirectServerGateway>> gateways;
    bots.reserve(kPlayers - 1);
    gateways.reserve(kPlayers - 1);

    // seat 0 由 ClientController 控制（经过 LocalLink）；其余座位用本地 bot。
    link.clientSide().send(Command::JoinRequest{.uid = 0});
    for (int i = 1; i < kPlayers; ++i) {
        auto bot = std::make_unique<AutoClient>();
        auto gateway = std::make_unique<DirectServerGateway>(
            session.controller, static_cast<client_id_t>(i + 1), *bot);
        bot->setGateway(*gateway);
        gateway->send(Command::JoinRequest{.uid = static_cast<unsigned>(i)});
        bots.push_back(std::move(bot));
        gateways.push_back(std::move(gateway));
    }
    link.clientSide().send(Command::StartGame{.big_blind = 10, .initial_chips = 200});

    link.pump();

    std::cout << std::format("[Loopback] idle={} stage={}\n", link.idle(),
                             to_string(session.model.getCurrentStage()));

    std::cout << "[Loopback] client view:\n";
    for (std::size_t i = 0; i < model.players.size(); ++i) {
        std::cout << std::format("  @P{} chips={} bet={} state={}\n", i,
                                 model.players[i].getChips(), model.player_bets[i],
                                 to_string(model.players[i].getState()));
    }

    std::cout << "[Loopback] server state:\n";
    for (seat_t seat = 0; seat < session.player_set.playerCount(); ++seat) {
        const Player& player = session.player_set.playerAt(seat);
        std::cout << std::format("  @P{} chips={} state={}\n", seat, player.getChips(),
                                 to_string(player.getState()));
    }
    return 0;
}
