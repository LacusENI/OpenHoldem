#include <algorithm>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ClientController.h"
#include "ClientModel.h"
#include "Deck.hpp"
#include "Event.hpp"
#include "GameTypes.hpp"
#include "IClient.h"
#include "IServer.h"
#include "LocalLink.h"
#include "NullClientView.h"
#include "Player.hpp"
#include "ServerSession.h"

using namespace poker;

namespace {
/** 简单机器人：轮到自己座位时 check/call（短筹码即 all-in 跟注）。 */
class AutoClient final : public NullClient {
public:
    explicit AutoClient(IServer& server) : server(server) {}

    void onJoinResponse(const Event::JoinResponse& event) override { seat = event.seat; }

    void onTurnStarted(const Event::TurnStarted& event) override {
        if (event.player != seat) return;
        const chips_t target_bet =
            (event.round_bet <= event.player_bet)
                ? event.player_bet
                : std::min(event.round_bet, event.player_bet + event.player_chips);
        server.onSubmit(Command::SubmitAction{.target_bet = target_bet, .is_fold = false});
    }

private:
    IServer& server;
    seat_t seat = INVALID_SEAT;
};
} // namespace

int main(int argc, char** argv) {
    std::uint32_t seed = Deck::randomSeed();
    if (argc > 1) seed = static_cast<std::uint32_t>(std::stoul(argv[1]));
    Deck deck(seed);
    std::cout << std::format("[Loopback] deck seed = {}\n", deck.getSeed());

    GameSession session(deck);
    ClientModel model;
    NullClientView view;

    LocalLink link;
    ClientController client(link.clientSide(), model, view);
    link.attach(session.controller, client);

    constexpr int kPlayers = 3;
    std::vector<std::unique_ptr<AutoClient>> bots;
    bots.reserve(kPlayers - 1);

    // seat 0 由 ClientController 控制（经过 LocalLink）；其余座位用本地 bot。
    link.clientSide().onJoin(link.serverSide(), Command::JoinRequest{.uid = 0});
    for (int i = 1; i < kPlayers; ++i) {
        bots.push_back(std::make_unique<AutoClient>(link.clientSide()));
        link.clientSide().onJoin(*bots.back(),
                                 Command::JoinRequest{.uid = static_cast<unsigned>(i)});
    }
    link.clientSide().onStart(Command::StartGame{.big_blind = 10, .initial_chips = 200});

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
