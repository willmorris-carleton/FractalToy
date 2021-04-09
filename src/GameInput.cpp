#include <GameInput.h>

void GameInput::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	updateKey(key, action);
}

void GameInput::updateKey(int key, int action)
{
	keys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT) ? true : false;
}

float GameInput::getKey(int keyCode)
{
	if (keys[keyCode] == NULL) return false;
	return keys[keyCode];
}
