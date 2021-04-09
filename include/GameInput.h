/*
Description:
	Contains the geometry vertex attributes for each shape.
*/
#ifndef GAMEINPUT_H_
#define GAMEINPUT_H_

#include <GL/glfw3.h>
#include <map>

//The geometry class contains the vertex attributes, the type
// and the name of the shape to create
class GameInput {

public:
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	float getKey(int keyCode);

private:
	void updateKey(int key, int action);
	std::map<int, bool> keys;

};

#endif

