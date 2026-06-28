#include "HandValue.h"

#include <format>

namespace poker {

std::string to_string(HandType hand_type) {
    switch (hand_type) {
    case HandType::HIGH_CARD:
        return "HIGH_CARD";
    case HandType::ONE_PAIR:
        return "ONE_PAIR";
    case HandType::TWO_PAIR:
        return "TWO_PAIR";
    case HandType::THREE_OF_A_KIND:
        return "THREE_OF_A_KIND";
    case HandType::STRAIGHT:
        return "STRAIGHT";
    case HandType::FLUSH:
        return "FLUSH";
    case HandType::FULL_HOUSE:
        return "FULL_HOUSE";
    case HandType::FOUR_OF_A_KIND:
        return "FOUR_OF_A_KIND";
    case HandType::STRAIGHT_FLUSH:
        return "STRAIGHT_FLUSH";
    case HandType::ROYAL_FLUSH:
        return "ROYAL_FLUSH";
    default:
        return "UNKNOWN";
    }
}

HandValue HandValue::build(HandType hand_type, std::vector<Rank> ranks) {
    int handValue = static_cast<int>(hand_type);
    int i = 0;
    for (; i < ranks.size(); i++) {
        int rank = static_cast<int>(ranks[i]);
        handValue = (handValue << 4) + rank;
    }
    for (; i < 5; i++) {
        handValue = handValue << 4;
    }
    return HandValue(handValue);
}

std::string to_string(HandValue hand_value) {
    return std::format("{}|{} {} {} {} {}", to_string(hand_value.getHandType()),
                       to_string(hand_value.getRank(0)), to_string(hand_value.getRank(1)),
                       to_string(hand_value.getRank(2)), to_string(hand_value.getRank(3)),
                       to_string(hand_value.getRank(4)));
}

std::ostream& operator<<(std::ostream& os, const HandValue& hand_value) {
    return os << to_string(hand_value);
}
} // namespace poker