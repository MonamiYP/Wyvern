#include "core/glfw/Input.hpp"
#include "core/Logger.hpp"
#include "core/Application.hpp"
#include "events/EventTypes.hpp"

std::array<bool, Input::MAX_KEYS> Input::current_keys{};
std::array<bool, Input::MAX_KEYS> Input::previous_keys{};

std::array<bool, Input::MAX_MOUSE> Input::current_mouse{};
std::array<bool, Input::MAX_MOUSE> Input::previous_mouse{};

void Input::update() {
    previous_keys = current_keys;
    previous_mouse = current_mouse;
}

void Input::processKey(int key, int action) {
    if (!validKey(key)) { Logger::error("Not a valid keyboard input"); return; }

    KeyPressedEvent e(key, action);
    Application::get().getEventDispatcher()->dispatch(e);

    if (action == GLFW_PRESS) current_keys[key] = true;
    else if (action == GLFW_RELEASE) current_keys[key] = false;
}