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
    std::unordered_map<std::type_index, std::vector<Handler>> handlers_map;

public:
    EventBus() = default;
    ~EventBus() = default;

    /**
     * 订阅事件
     * @tparam T 事件类型
     * @param callback 回调函数
     */
    template <typename T>
    void subscribe(std::function<void(const T&)> callback) {
        handlers_map[typeid(T)].push_back(
            [callback](const void* eventPtr) { callback(*static_cast<const T*>(eventPtr)); });
    }

    /**
     * 发布事件
     * @tparam T 事件类型
     * @param event 事件
     */
    template <typename T>
    void publish(const T& event) {
        for (auto& handler : handlers_map[typeid(T)]) {
            handler(&event);
        }
    }
};
} // namespace poker