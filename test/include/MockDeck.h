#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Deck.h"

class MockDeck : public poker::IDeck {
public:
    MOCK_METHOD(void, shuffle, (), (override));
    MOCK_METHOD(poker::Card, deal, (), (override));
};

class MockEmptyDeck : public poker::IDeck {
public:
    void shuffle() override {}

    poker::Card deal() override { return poker::Card(); }
};
