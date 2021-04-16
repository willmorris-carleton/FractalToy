/*
Description:
	Controls a camera which is clamped as to not allow rolling when looking around
*/
#ifndef FPSCAM_H
#define FPSCAM_H

#include <camera.h>

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

