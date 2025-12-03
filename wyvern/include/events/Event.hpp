#pragma once

#include <string>
#include <typeindex>
#include "core/Logger.hpp"

struct Event {
    public:
        virtual ~Event() = default;
};

class EventDispatcher {
    public:
        using EventCallbackFn = std::function<bool(Event&)>; // Input event, output is true if event is handled fully
        using EventID = std::type_index;

        template<typename EventType>
        void registerListener(EventCallbackFn callback) {
            auto& listeners = m_listeners[typeid(EventType)];
            listeners.push_back(callback);
            Logger::debug("Added listener to event: %s", typeid(EventType).name());
        }

        template<typename EventType>
        void unregisterListener(EventCallbackFn callback) {
            // To implement
        }

        void dispatch(Event& event) {
            // Logger::debug("Firing event: %s", typeid(event).name());
            auto it = m_listeners.find(typeid(event));
            if (it != m_listeners.end()) {
                for (auto& listener : it->second) {
                    if (listener(event)) break; // stop propagation if handled
                }
            }
        }

    private:
        std::unordered_map<EventID, std::vector<EventCallbackFn>> m_listeners;
};