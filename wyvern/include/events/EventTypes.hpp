#pragma once

#include "events/Event.hpp"

struct KeyPressedEvent : Event { int key; };

struct WindowResizeEvent : Event { int width, height; };

struct WindowCloseEvent : Event { };