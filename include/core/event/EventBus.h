#pragma once
#include <functional>
#include <typeindex>

namespace poker {
using Handler = std::function<void(const void*)>;

/**
 * @brief 事件总线
 */
class EventBus {
private:
    static std::unordered_map<std::type_index, std::vector<Handler>>& getHandlers() {
        static std::unordered_map<std::type_index, std::vector<Handler>> handlers;
        return handlers;
    }

public:
    /**
     * 订阅事件
     * @tparam T 事件类型
     * @param callback 回调函数
     */
    template <typename T>
    static void subscribe(std::function<void(const T&)> callback) {
        getHandlers()[typeid(T)].push_back(
            [callback](const void* eventPtr) { callback(*static_cast<const T*>(eventPtr)); });
    }

    /**
     * 发布事件
     * @tparam T 事件类型
     * @param event 事件
     */
    template <typename T>
    static void publish(const T& event) {
        auto& handlers = getHandlers()[typeid(T)];
        for (auto& handler : handlers) {
            handler(&event);
        }
    }
};
} // namespace poker