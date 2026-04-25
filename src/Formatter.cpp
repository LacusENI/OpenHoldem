#include <format>

#include "Formatter.h"
#include "entity/Card.h"
#include "entity/HandValue.h"
#include "entity/GameTypes.h"

namespace holdem::ui::formatter {
std::string format(Rank rank) {
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

std::string format(Suit suit) {
    static std::vector<std::string> suits = {
        "_", "\u2663", "\u2666", "\u2665", "\u2660"
    }; // None, Club, Diamond, Heart, Spade
    return suits[static_cast<int>(suit)];
}

std::string format(const Card& card) {
    return format(card.suit) + utils::castRankToString(card.rank);
}

std::string format(const HandType& hand_type) {
    switch (hand_type) {
    case HandType::ROYAL_FLUSH: return "Royal Flush";
    case HandType::STRAIGHT_FLUSH: return "Straight Flush";
    case HandType::FOUR_OF_A_KIND: return "Four of a Kind";
    case HandType::FULL_HOUSE: return "Full House";
    case HandType::FLUSH: return "Flush";
    case HandType::STRAIGHT: return "Straight";
    case HandType::THREE_OF_A_KIND: return "Three of a Kind";
    case HandType::TWO_PAIR: return "Two Pair";
    case HandType::ONE_PAIR: return "One Pair";
    case HandType::HIGH_CARD: return "High Card";
    default: return "???";
    }
}

std::string format(const HandValue& hand_value) {
    HandType hand_type = hand_value.getHandType();
    std::string x, y;
    std::string description;
    switch (hand_type) {
    case HandType::ROYAL_FLUSH:
        break;
    case HandType::STRAIGHT_FLUSH:
        x = format(hand_value.getRank(4));
        y = format(hand_value.getRank(0));
        description = std::format(
            "{} to {}", x, y);
        break;
    case HandType::FOUR_OF_A_KIND:
        x = format(hand_value.getRank(0));
        description = std::format(
            "{}s", x);
        break;
    case HandType::FULL_HOUSE:
        x = format(hand_value.getRank(0));
        y = format(hand_value.getRank(1));
        description = std::format(
            "{}s full of {}s", x, y);
        break;
    case HandType::FLUSH:
        x = format(hand_value.getRank(0));
        description = std::format(
            "{}-high", x);
        break;
    case HandType::STRAIGHT:
        x = format(hand_value.getRank(4));
        y = format(hand_value.getRank(0));
        description = std::format(
            "{} to {}", x, y);
        break;
    case HandType::THREE_OF_A_KIND:
        x = format(hand_value.getRank(0));
        description = std::format(
            "{}s", x);
        break;
    case HandType::TWO_PAIR:
        x = format(hand_value.getRank(0));
        y = format(hand_value.getRank(1));
        description = std::format(
            "{}s and {}s", x, y);
        break;
    case HandType::ONE_PAIR:
        x = format(hand_value.getRank(0));
        description = std::format(
            "{}s", x);
        break;
    case HandType::HIGH_CARD:
        x = format(hand_value.getRank(0));
        description = std::format(
            "{}-high", x);
        break;
    default: description = "???";
    }
    return std::format("{}, {}", format(hand_type), description);
}

std::string format(const ActionType& action_type) {
    switch (action_type) {
    case ActionType::CALL:
        return "Call";
    case ActionType::CHECK:
        return "Check";
    case ActionType::RAISE:
        return "Raise";
    case ActionType::BET:
        return "Bet";
    case ActionType::FOLD:
        return "Fold";
    case ActionType::ALL_IN:
        return "All-in";
    case ActionType::SMALL_BLIND:
        return "Small Blind";
    case ActionType::BIG_BLIND:
        return "Big Blind";
    default:
        return "???";
    }
}
}