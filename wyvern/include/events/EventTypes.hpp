#pragma once

#include "events/Event.hpp"

struct KeyPressedEvent : Event { 
    int key; 
    int action;
    KeyPressedEvent(int _key, int _action) : key(_key), action(_action) {}
};

struct WindowResizeEvent : Event { 
    int width, height; 
    WindowResizeEvent(int w, int h) : width(w), height(h) {}
};

struct WindowCloseEvent : Event { };