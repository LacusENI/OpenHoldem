#pragma once

namespace poker {
class ClientModel;

class ClientView {
public:
    ClientView() = default;
    virtual ~ClientView() = default;
    virtual void render(const ClientModel& client_model) = 0;
};
} // namespace poker