#ifndef OPENHOLDEM_MOCK_DECK_H
#define OPENHOLDEM_MOCK_DECK_H
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "models/Deck.h"

class MockDeck : public holdem::IDeck {
public:
    MOCK_METHOD(void, shuffle, (), (override));
    MOCK_METHOD(holdem::Card, deal, (), (override));
};

class MockEmptyDeck : public holdem::IDeck {
public:
    void shuffle() override {}

    holdem::Card deal() override { return holdem::Card(); }
};

#endif //OPENHOLDEM_MOCK_DECK_H