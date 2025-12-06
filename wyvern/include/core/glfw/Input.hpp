#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <array>

enum class MouseCode {
    Button0                = 0,
    Button1                = 1,
    Button2                = 2,
    Button3                = 3,
    Button4                = 4,
    Button5                = 5,
    Button6                = 6,
    Button7                = 7,

    ButtonLast             = Button7,
    ButtonLeft             = Button0,
    ButtonRight            = Button1,
    ButtonMiddle           = Button2
};

class Input {
    public:
        static void update();

        // Keyboard input
        static bool isKeyPressed(int key) { return current_keys[key]; }
        static bool wasKeyPressed(int key) { return previous_keys[key]; };
        static bool isKeyHeld(int key) { return current_keys[key] && previous_keys[key]; }
        static bool wasKeyReleased(int key) { return !current_keys[key] && previous_keys[key]; };
        static void processKey(int key, int action);

        // Mouse input
        static bool isMouseButtonPressed(MouseCode button);
        static bool wasMouseButtonPressed(MouseCode button);
        static bool isMouseButtonHeld(MouseCode button);
        static bool wasMouseButtonHeld(MouseCode button);
		static glm::vec2 getMousePosition();
        static glm::vec2 getPrevMousePosition();
		static float getMouseX() { return getMousePosition().x; }
		static float getMouseY() { return getMousePosition().y; }
        static float getPrevMouseX() { return getPrevMousePosition().x; }
		static float getPrevMouseY() { return getPrevMousePosition().y; }
        static void processButton(MouseCode button, int action);

    private:
        static inline bool validKey(int key) { return key >= 0 && key < MAX_KEYS; }
        static inline bool validMouse(int btn) { return btn >= 0 && btn < MAX_MOUSE; }

        static const int MAX_KEYS = GLFW_KEY_LAST + 1;
        static const int MAX_MOUSE = 4;
        static std::array<bool, MAX_KEYS> current_keys;
        static std::array<bool, MAX_KEYS> previous_keys;

        static std::array<bool, MAX_MOUSE> current_mouse;
        static std::array<bool, MAX_MOUSE> previous_mouse;
};