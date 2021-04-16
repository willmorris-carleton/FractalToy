/*
Description:
	Helper class to get time and deltaTime easily anwhere in program
*/
#ifndef GAMETIME_H_
#define GAMETIME_H_

#include <GL/glfw3.h>

class GameTime {

public:
	GameTime();
	void update();
	float getDelta();
	int getFPS();

	static GameTime* gt;
private:
	float lastTime;

};

#endif

