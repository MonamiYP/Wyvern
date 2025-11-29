#include "core/glfw/Input.hpp"
#include "core/Application.hpp"

bool Input::isKeyPressed(KeyCode key) {
    GLFWwindow* window = Application::get().getWindow()->getNativeWindow();
    int state = glfwGetKey(window, static_cast<int>(key));
    return state == GLFW_PRESS;
}

bool Input::isMouseButtonPressed(MouseCode button) {
    GLFWwindow* window = Application::get().getWindow()->getNativeWindow();
    int state = glfwGetKey(window, static_cast<int>(button));
    return state == GLFW_PRESS;
}

glm::vec2 Input::getMousePosition() {
    GLFWwindow* window = Application::get().getWindow()->getNativeWindow();
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return glm::vec2(xpos, ypos);
}