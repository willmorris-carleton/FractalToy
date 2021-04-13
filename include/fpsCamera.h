/*
Description:
	Controls a camera which is clamped as to not allow rolling of camera
*/
#ifndef FPSCAM_H
#define FPSCAM_H

#include <camera.h>

//The geometry class contains the vertex attributes, the type
// and the name of the shape to create
class FPSCamera : public Camera {

public:
	FPSCamera();
	virtual int Pitch(float angleDeg);
	virtual int Yaw(float angleDeg);
	
	static glm::vec3 WorldUp;
	static Camera* currentCam;

	static float movementSpeed;

private:
	float curPitch = 0;
};

#endif

