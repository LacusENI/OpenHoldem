#include "ConsoleView.h"

#include <format>
#include <iostream>

#include "ClientModel.h"
#include "Formatter.h"

namespace poker::ui {
void ConsoleView::render(const ClientModel& client_model) {
    using formatter::format;
    std::cout << std::format("BTN=@P{} Pot: ${} Community Cards: {} \n", client_model.btn_seat + 1,
                             client_model.pot, format(client_model.community_cards));
    for (const Player& player : client_model.players) {
        std::string flag = player.position == client_model.this_player_seat ? "(You)" : "";
        std::cout << std::format("[@P{}] {:5} ${}, {}\n", player.position + 1, flag,
                                 client_model.player_bets[player.position],
                                 format(client_model.player_states[player.position]));
        std::cout << std::format("${}, {} | \n", player.chips, format(player.hole_cards));
    }
}

} // namespace poker::ui