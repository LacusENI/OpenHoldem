#pragma once
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

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

    template <typename EventType, typename ClassType>
    void subscribe(void(ClassType::*mem_func)(const EventType&), ClassType* instance) {
        this->subscribe<EventType>([instance, mem_func](const EventType& event) {
            (instance->*mem_func)(event);
        });
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