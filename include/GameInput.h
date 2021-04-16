/*
Description:
	Class to keep track of which inputs are currently down at the same time.
*/
#ifndef GAMEINPUT_H_
#define GAMEINPUT_H_

#include <GL/glfw3.h>
#include <map>

class GameInput {

public:
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	float getKey(int keyCode);

private:
	void updateKey(int key, int action);
	std::map<int, bool> keys;

};

#endif

