#include <GameTime.h>

GameTime::GameTime() {
	update();
}

void GameTime::update() {
	lastTime = glfwGetTime();
}

float GameTime::getDelta()
{
	return glfwGetTime() - lastTime;;
}

int GameTime::getFPS()
{
	return (int) (1 / getDelta());
}
