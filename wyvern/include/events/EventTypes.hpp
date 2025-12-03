#pragma once

#include "events/Event.hpp"

struct KeyPressedEvent : Event { int key; };

struct WindowResizeEvent : Event { 
    int width, height; 
    WindowResizeEvent(int w, int h) : width(w), height(h) {}
};

struct WindowCloseEvent : Event { };