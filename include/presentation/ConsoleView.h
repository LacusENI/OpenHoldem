#pragma once
#include "ClientView.h"

namespace poker::ui {
class ConsoleView : public ClientView {
public:
    void render(const ClientModel& client_model) override;
};
} // namespace poker