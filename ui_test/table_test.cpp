#include "ClientModel.h"
#include "ConsoleView.h"
#include "ui_test.h"

using namespace poker;

void test_table() {
    ClientModel client_model;
    client_model.community_cards = {Card("D8"), Card("CT"), Card("SA")};
    Player player1 = {.position = 0, .hole_cards = {}, .chips = 230};
    Player player2 = {.position = 1, .hole_cards = {}, .chips = 140, .is_folded = true};
    Player player3 = {.position = 2, .hole_cards = {}, .chips = 190};
    Player player4 = {.position = 3, .hole_cards = {}, .chips = 60};
    Player player5 = {.position = 4, .hole_cards = {Card("CK"), Card("DA")}, .chips = 75};
    client_model.players = {player1, player2, player3, player4, player5};
    client_model.player_states = {PlayerStatus::BIG_BLIND, PlayerStatus::FOLDED,
                                  PlayerStatus::CALLED, PlayerStatus::RAISED, PlayerStatus::CALLED};
    client_model.player_bets = {230, 140, 190, 60, 75};
    client_model.btn_seat = 3;
    client_model.this_player_seat = 4;
    client_model.big_blind = 10;
    client_model.pot = 105;
    ui::ConsoleView console_view;
    console_view.render(client_model);
}