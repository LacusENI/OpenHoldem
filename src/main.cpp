#include <cstdint>
#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "ClientGateway.h"
#include "Deck.hpp"
#include "Event.hpp"
#include "EventBus.hpp"
#include "GameSession.h"
#include "GameTypes.hpp"
#include "Player.hpp"

using namespace poker;

void printEvent(const Event::BlindPosted& event) {
    std::cout << std::format("[Event] Blind Posted | @P{} ${}\n", event.player, event.amount);
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

void printEvent(const Event::PotAwarded event) {
    std::string msg;
    for (seat_t player : event.award_to) {
        msg += std::format("@P{} ", player);
    }
    std::cout << std::format("[Event] PotAwared | {} ${} {}\n", event.pot_index, event.chips, msg);
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

int main(int argc, char** argv) {
    std::uint32_t seed = Deck::randomSeed();
    if (argc > 1) {
        seed = static_cast<std::uint32_t>(std::stoul(argv[1]));
    }
    Deck deck(seed);
    std::cout << std::format("[Demo] deck seed = {} (可用 ./poker {} 复现)\n", deck.getSeed(),
                             deck.getSeed());

    ClientGateway client_gateway;
    GameSession session(deck, client_gateway);
    EventBus& event_bus = session.event_bus;
    PlayerSet& player_set = session.player_set;
    GameModel& game_model = session.model;
    GameController& game_controller = session.controller;

    game_controller.subscribeEvent();

    event_bus.subscribe<Event::TurnStarted>(
        [&game_model](const auto& _) { printGameModel(game_model); });
    event_bus.subscribe<Event::GameEnded>(
        [&game_model](const auto& _) { printGameModel(game_model); });
    event_bus.subscribe<Event::BlindPosted>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::TurnStarted>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::PlayerActed>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::PotAwarded>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::UncalledBetReturned>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::HoleCardsRevealed>([](const auto& event) { printEvent(event); });

    event_bus.subscribe<Event::TurnStarted>(
        [&event_bus](const auto& event) { event_bus.publish(getPlayerInput(event)); });

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

    return 0;
}