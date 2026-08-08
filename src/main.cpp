#include <exception>
#include <format>
#include <iostream>
#include <string>

#include "ActionFlow.h"
#include "BettingManager.h"
#include "CardsManager.h"
#include "ChipsManager.h"
#include "Deck.hpp"
#include "Event.hpp"
#include "EventBus.hpp"
#include "GameController.h"
#include "GameModel.h"
#include "GameTypes.hpp"
#include "Player.hpp"
#include "PlayerSet.hpp"
#include "PotManager.h"
#include "SeatManager.h"

using namespace poker;

void printEvent(const Event::BlindPosted& event) {
    std::cout << std::format("[Event] Blind Posted | @P{} ${}\n", event.player, event.amount);
}

void printEvent(const Event::WaitingForAction& event) {
    std::cout << std::format(
        "[Event] Waiting For Action | "
        "@P{} chips:${} bet:${} round-bet:${} min-raise:${}\n",
        event.player, event.player_chips, event.player_bet, event.round_bet, event.min_raise);
}

void printEvent(const Event::PlayerActed& event) {
    std::cout << std::format("[Event] Player Acted | @P{} ${} {}\n", event.player, event.bet,
                             to_string(event.action));
}

void printEvent(const Event::WinnerDeclared& event) {
    std::string msg;
    for (seat_t player : event.winners) {
        msg += std::format("@P{} ", player);
    }
    std::cout << std::format("[Event] Winner | {}\n", msg);
}

void printGameModel(const GameModel& model) {
    std::cout << std::format(
        "stage: {} community-cards: {} pot: ${}\n", to_string(model.getCurrentStage()),
        to_string(model.getCardsManager()->getCommunityCards()), model.getPotManager()->getPot());
    std::cout << std::format("round-bet: ${} min-raise: ${}\n",
                             model.getBettingManager()->getRoundBet(),
                             model.getBettingManager()->getRoundBet());
    seat_t player_seat = 0;
    for (const Player& player : *model.getPlayerSet()) {
        std::cout << std::format("[@P{}] {} {} bet: ${} chips: ${}\n", player_seat,
                                 to_string(player.getHoleCards()), to_string(player.getState()),
                                 player.getBet(), player.getChips());
        player_seat++;
    }
}

Event::PlayerAction getPlayerInput(const Event::WaitingForAction& event) {
    while (true) {
        std::cout << std::format("@P{} >>> ", event.player);
        std::string input;
        std::cin >> input;
        if (input == "f") {
            return Event::PlayerAction{.target_bet = 0, .is_fold = true, .is_all_in = false};
        } else {
            try {
                int target_bet = std::stoi(input);
                if (target_bet - event.player_bet > event.player_chips) {
                    std::cout << std::format("Beyond the chips you have(${}).\n",
                                             event.player_chips);
                    continue;
                }
                if (target_bet - event.player_bet == event.player_chips) {
                    return Event::PlayerAction{
                        .target_bet = target_bet, .is_fold = false, .is_all_in = true};
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
                    return Event::PlayerAction{
                        .target_bet = target_bet, .is_fold = false, .is_all_in = false};
                }
            } catch (const std::exception& e) {
            }
        }
    }
}

int main() {
    EventBus event_bus;

    Deck deck;
    PlayerSet player_set;
    PotManager pot_manager;
    SeatManager seat_manager(&player_set);
    BettingManager betting_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);
    CardsManager cards_manager(&deck, &player_set);
    ChipsManager chips_manager(&player_set, &pot_manager, &seat_manager);
    GameModel game_model(&event_bus, &action_flow, &betting_manager, &cards_manager, &chips_manager,
                         &player_set, &pot_manager, &seat_manager);
    GameController game_controller(&event_bus, &game_model);

    game_controller.subscribeEvent();

    event_bus.subscribe<Event::WaitingForAction>(
        [&game_model](const auto& _) { printGameModel(game_model); });
    event_bus.subscribe<Event::GameEnded>(
        [&game_model](const auto& _) { printGameModel(game_model); });
    event_bus.subscribe<Event::BlindPosted>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::WaitingForAction>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::PlayerActed>([](const auto& event) { printEvent(event); });
    event_bus.subscribe<Event::WinnerDeclared>([](const auto& event) { printEvent(event); });

    event_bus.subscribe<Event::WaitingForAction>(
        [&event_bus](const auto& event) { event_bus.publish(getPlayerInput(event)); });

    event_bus.publish(Event::PlayerJoinRequest{});
    event_bus.publish(Event::PlayerJoinRequest{});
    event_bus.publish(Event::PlayerJoinRequest{});
    event_bus.publish(Event::PlayerJoinRequest{});
    event_bus.publish(Event::PlayerJoinRequest{});

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(100, 300);
    for (Player& player : player_set) {
        player.setChips(dis(gen));
    }

    event_bus.publish(Event::StartGame{});

    return 0;
}