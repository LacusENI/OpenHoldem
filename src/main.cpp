#include <cstdint>
#include <exception>
#include <format>
#include <iostream>
#include <queue>
#include <string>
#include <type_traits>
#include <variant>

#include "ClientGateway.h"
#include "Deck.hpp"
#include "Event.hpp"
#include "EventBus.hpp"
#include "GameTypes.hpp"
#include "Player.hpp"
#include "ServerSession.h"

using namespace poker;

void printEvent(const Event::GameStarted& event) {
    std::cout << std::format("[Event] Game Started | btn:@P{} bb:${} players:{}\n", event.btn_seat,
                             event.big_blind, event.player_count);
}

void printEvent(const Event::PlayerJoined& event) {
    std::cout << std::format("[Event] Player Joined | @P{} players:{}\n", event.seat,
                             event.player_count);
}

void printEvent(const Event::JoinResponse& event) {
    std::cout << std::format("[Event] Join Response | seat:@P{}\n", event.seat);
}

void printEvent(const Event::StageChanged& event) {
    std::cout << std::format("[Event] Stage Changed | {}\n", to_string(event.stage));
}

void printEvent(const Event::BlindPosted& event) {
    std::cout << std::format("[Event] Blind Posted | @P{} ${}\n", event.player, event.amount);
}

void printEvent(const Event::HoleCardsDealt& event) {
    std::cout << std::format("[Event] Hole Cards Dealt | @P{} {}\n", event.player_seat,
                             to_string(event.hole_cards));
}

void printEvent(const Event::CommunityCardsDealt& event) {
    std::cout << std::format("[Event] Community Cards | {}\n", to_string(event.community_cards));
}

void printEvent(const Event::TurnStarted& event) {
    std::cout << std::format(
        "[Event] Waiting For Action | "
        "@P{} chips:${} bet:${} round-bet:${} min-raise:${} can-raise:{}\n",
        event.player, event.player_chips, event.player_bet, event.round_bet, event.min_raise,
        event.can_raise);
}

void printEvent(const Event::PlayerActed& event) {
    std::cout << std::format("[Event] Player Acted | @P{} ${} {}{}\n", event.player, event.bet,
                             to_string(event.action), event.is_all_in ? " (all-in)" : "");
}

void printEvent(const Event::PotAwarded& event) {
    std::string msg;
    for (seat_t player : event.award_to) {
        msg += std::format("@P{} ", player);
    }
    std::cout << std::format("[Event] Pot Awarded | {} ${} {}\n", event.pot_index, event.chips, msg);
}

void printEvent(const Event::UncalledBetReturned& event) {
    std::cout << std::format("[Event] Uncalled Bet Returned | @P{} ${}\n", event.player,
                             event.amount);
}

void printEvent(const Event::HoleCardsRevealed& event) {
    for (const auto& [seat, cards] : event.players_with_hole_cards) {
        std::cout << std::format("[Event] Reveal | @P{} {}\n", seat, to_string(cards));
    }
}

void printEvent(const Event::GameEnded&) { std::cout << "[Event] Game Ended\n"; }

/* 兜底：未专门格式化的事件不打印 */
template <typename T>
void printEvent(const T&) {}

void printGameModel(const GameModel& model) {
    std::cout << std::format("stage: {} community-cards: {}\n", to_string(model.getCurrentStage()),
                             to_string(model.getCardsManager().getCommunityCards()));

    std::cout << std::format("round-bet: ${} min-raise: ${}\n",
                             model.getBettingRound().getRoundBet(),
                             model.getBettingRound().getMinRaise());
    seat_t player_seat = 0;
    for (const Player& player : model.getPlayerSet()) {
        std::cout << std::format("[@P{}] {} {} bet: ${} chips: ${} committed: ${}\n", player_seat,
                                 to_string(player.getHoleCards()), to_string(player.getState()),
                                 player.getBet(), player.getChips(), player.getCommitted());
        player_seat++;
    }
}

Command::SubmitAction getPlayerInput(const Event::TurnStarted& event) {
    while (true) {
        std::cout << std::format("@P{} >>> ", event.player);
        std::string input;
        std::cin >> input;
        if (input == "f") {
            return {.target_bet = 0, .is_fold = true};
        } else {
            try {
                int target_bet = std::stoi(input);
                if (target_bet - event.player_bet > event.player_chips) {
                    std::cout << std::format("You don't have enough chips(${} to all-in).\n",
                                             event.player_chips + event.player_bet);
                    continue;
                }
                if (target_bet > event.round_bet && !event.can_raise) {
                    std::cout << "You can't raise now.\n";
                    continue;
                }
                if (target_bet - event.player_bet == event.player_chips) {
                    return {.target_bet = target_bet, .is_fold = false};
                }
                if (target_bet < event.round_bet) {
                    std::cout << std::format("Under the call(${}).\n", event.round_bet);
                    continue;
                }
                if (target_bet > event.round_bet && target_bet < event.min_raise) {
                    std::cout << std::format("Under the min-raise(${}).\n", event.min_raise);
                    continue;
                }
                if (target_bet == event.round_bet ||
                    (target_bet > event.round_bet && target_bet >= event.min_raise)) {
                    return {.target_bet = target_bet, .is_fold = false};
                }
            } catch (const std::exception& e) {
            }
        }
    }
}

/**
 * @brief 控制台出站实现
 * @details 广播/单传都直接打印；轮到玩家行动时先读入，再把动作入队，
 *          由 main 的循环统一 publish，避免在事件回调里同步重入。
 */
class ConsoleClientGateway final : public ClientGateway {
public:
    void broadcast(const GameEvent& event) override {
        std::visit([](const auto& e) { printEvent(e); }, event);
    }

    void sendTo(seat_t seat, const GameEvent& event) override {
        (void)seat;
        std::visit(
            [this](const auto& e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<T, Event::TurnStarted>) {
                    if (model != nullptr) printGameModel(*model);
                    printEvent(e);
                    pending.push(getPlayerInput(e));
                } else {
                    printEvent(e);
                }
            },
            event);
    }

    void setModel(const GameModel* value) { model = value; }

    [[nodiscard]] bool hasPending() const { return !pending.empty(); }

    Command::SubmitAction popPending() {
        Command::SubmitAction action = pending.front();
        pending.pop();
        return action;
    }

private:
    const GameModel* model = nullptr;
    std::queue<Command::SubmitAction> pending;
};

int main(int argc, char** argv) {
    std::uint32_t seed = Deck::randomSeed();
    if (argc > 1) {
        seed = static_cast<std::uint32_t>(std::stoul(argv[1]));
    }
    Deck deck(seed);
    std::cout << std::format("[Demo] deck seed = {} (可用 ./poker {} 复现)\n", deck.getSeed(),
                             deck.getSeed());

    ConsoleClientGateway client_gateway;
    GameSession session(deck, client_gateway);
    client_gateway.setModel(&session.model);

    EventBus& event_bus = session.event_bus;
    PlayerSet& player_set = session.player_set;
    ServerController& server_controller = session.controller;

    server_controller.subscribeEvent();

    event_bus.publish(Command::JoinRequest{});
    event_bus.publish(Command::JoinRequest{});
    event_bus.publish(Command::JoinRequest{});
    event_bus.publish(Command::JoinRequest{});
    event_bus.publish(Command::JoinRequest{});

    std::mt19937 gen(seed); // 与牌堆同一种子，便于整体复现
    std::uniform_int_distribution<int> dis(100, 300);
    for (Player& player : player_set) {
        player.setChips(dis(gen));
    }

    event_bus.publish(Command::StartGame{});

    // 动作由 gateway 入队，这里统一发布；避免在事件回调里同步重入。
    while (client_gateway.hasPending()) {
        event_bus.publish(client_gateway.popPending());
    }

    return 0;
}
