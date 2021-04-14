#include <GameTime.h>

GameTime* GameTime::gt = nullptr;

GameTime::GameTime() {
	update();
	GameTime::gt = this;
}

void GameTime::update() {
	lastTime = glfwGetTime();
}

float GameTime::getDelta()
{
	return glfwGetTime() - lastTime;
}

int GameTime::getFPS()
{
	return (int) (1 / getDelta());
}
