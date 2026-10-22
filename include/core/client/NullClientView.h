#pragma once

#include "ClientView.h"

namespace poker {
/** @brief 空视图：不需要客户端界面时使用（回环测试 / 无头运行）。 */
class NullClientView final : public ClientView {
public:
    void render(const ClientModel&) override {}
};
} // namespace poker
