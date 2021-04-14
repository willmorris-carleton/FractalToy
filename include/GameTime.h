/*
Description:
	Contains the geometry vertex attributes for each shape.
*/
#ifndef GAMETIME_H_
#define GAMETIME_H_

#include <GL/glfw3.h>

//The geometry class contains the vertex attributes, the type
// and the name of the shape to create
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

