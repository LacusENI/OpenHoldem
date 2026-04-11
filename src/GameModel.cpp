#include "Card.h"
#include "GameModel.h"
#include "HandEvaluator.h"

namespace holdem::internal {
std::string rankToMessage(Rank rank) {
    switch (rank) {
    case Rank::ACE: return "Ace";
    case Rank::KING: return "King";
    case Rank::QUEEN: return "Queen";
    case Rank::JACK: return "Jack";
    case Rank::TEN: return "Ten";
    default:
        return std::to_string(static_cast<int>(rank));
    }
}

std::string getHandMessage(HandValue hand_value) {
    HandType hand_type = hand_value.getHandType();
    std::string x, y;
    switch (hand_type) {
    case HandType::ROYAL_FLUSH:
        return "Royal Flush";
    case HandType::STRAIGHT_FLUSH:
        x = rankToMessage(hand_value.getRank(4));
        y = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Straight Flush, {} to {}", x, y);
    case HandType::FOUR_OF_A_KIND:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Four of a Kind, {}s", x);
    case HandType::FULL_HOUSE:
        x = rankToMessage(hand_value.getRank(0));
        y = rankToMessage(hand_value.getRank(1));
        return std::format(
            "Full House, {}s full of {}s", x, y);
    case HandType::FLUSH:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Flush, {}-high", x);
    case HandType::STRAIGHT:
        x = rankToMessage(hand_value.getRank(4));
        y = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Straight, {} to {}", x, y);
    case HandType::THREE_OF_A_KIND:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Three of a Kind, {}s", x);
    case HandType::TWO_PAIR:
        x = rankToMessage(hand_value.getRank(0));
        y = rankToMessage(hand_value.getRank(1));
        return std::format(
            "Two pair, {}s and {}s", x, y);
    case HandType::ONE_PAIR:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "One Pair, {}s", x);
    case HandType::HIGH_CARD:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "High Card, {}-high", x);
    }
    return "";
}
} // namespace holdem::internal

namespace holdem {
void GameModel::addPlayer(PlayerId id) {
    Player new_player;
    new_player.id = id;
    new_player.position = static_cast<int>(players.size());
    players.push_back(new_player);
}

void GameModel::commitChips(Position position, Stack amount) {
    Player& player = getPlayer(position);
    player.chips -= amount;
    pot += amount;
    player.current_bet += amount;
    if (player.current_bet > round_bet) {
        round_bet = player.current_bet;
    }
}

void GameModel::dealCards() {
    switch (game_state) {
    case GameState::PREFLOP:
        for (Player& player : players) {
            player.hole_cards[0] = deck->deal();
            player.hole_cards[1] = deck->deal();
        }
        break;
    case GameState::FLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        break;
    case GameState::TURN:
        community_cards[3] = deck->deal();
        break;
    case GameState::RIVER:
        community_cards[4] = deck->deal();
        break;
    default: break;
    }
}

Action GameModel::bigBlind() {
    Position position = getBigBlindPosition();
    commitChips(position, big_blind);
    return {position, "Big Blind", big_blind};
}

Action GameModel::smallBlind() {
    Position position = getSmallBlindPosition();
    Stack small_blind = big_blind / 2;
    commitChips(position, small_blind);
    return {position, "Small Blind", small_blind};
}

Action GameModel::takeAction() {
    Player& player = getPlayer(current_position);
    std::string action_msg;
    Stack amount = 0;
    if (round_bet == 0) {
        action_msg = "Bet";
        amount = big_blind;
    } else if (player.current_bet < round_bet) {
        action_msg = "Call";
        amount = round_bet - player.current_bet;
    } else {
        action_msg = "Check";
    }
    commitChips(current_position, amount);
    return {current_position, action_msg, amount};
}

void GameModel::nextPlayer() {
    current_position = nextPosition(current_position);
    if (current_position == rest_position)
        is_round_ended = true;
}

std::vector<Stack> GameModel::award() {
    int winners_n = static_cast<int>(winners.size());
    int share = pot / winners_n;
    int remainder = pot % winners_n;

    std::vector<Stack> amounts;
    for (size_t i = 0; i < winners_n; ++i) {
        int amount = share;
        // 前 remainder 个玩家多得 1 个筹码
        if (i < remainder) amount += 1;

        amounts.push_back(amount);
        Player& player = getPlayer(winners[i]);
        player.chips += amount;
    }
    return amounts;
}

void GameModel::nextStreet() {
    for (Player& player : players) {
        player.current_bet = 0;
    }

    switch (game_state) {
    case GameState::IDLE:
        game_state = GameState::PREFLOP;
        current_position = getUtgPosition();
        break;
    case GameState::PREFLOP:
        game_state = GameState::FLOP;
        current_position = nextPosition(button_position);
        break;
    case GameState::FLOP:
        game_state = GameState::TURN;
        current_position = nextPosition(button_position);
        break;
    case GameState::TURN:
        game_state = GameState::RIVER;
        current_position = nextPosition(button_position);
        break;
    case GameState::RIVER:
        game_state = GameState::AWARD;
    default: break;
    }
    dealCards();
    rest_position = current_position;
    is_round_ended = false;
    round_bet = 0;
}

void GameModel::showdown() {
    // 评估每位玩家的手牌大小
    for (Player& player : players) {
        Cards7 hand7 = concatCards(player.hole_cards, community_cards);
        player.hand_value = HandEvaluator::evaluate(hand7);
    }
    // 决出手牌最大的获胜者
    HandValue winner_value;
    for (Player& player : players) {
        if (player.hand_value == winner_value) {
            winners.push_back(player.position);
        }
        else if (player.hand_value > winner_value) {
            winners.clear();
            winners.push_back(player.position);
            winner_value = player.hand_value;
        }
    }
}

void GameModel::runBettingRound() {
    while (!is_round_ended) {
        takeAction();
        nextPlayer();
    }
}

}