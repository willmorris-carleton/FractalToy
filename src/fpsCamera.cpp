#include "fpsCamera.h"
#include <iostream>

glm::vec3 FPSCamera::WorldUp = glm::vec3(0, 1, 0);

int FPSCamera::Pitch(float angleDeg)
{
	
	std::cout << angleDeg << std::endl;

	curPitch += angleDeg;

	float ang = 0;
	if (curPitch >= 90) {
		curPitch = 90;
	}
	else if (curPitch <= -90) {
		curPitch = -90;
	}
	else {
		ang = angleDeg;
	}
	
	std::cout << "Pitch: " << curPitch << std::endl;

	glm::quat rotation = glm::angleAxis(glm::radians(ang), GetSide());
	orientation_ *= rotation;
	orientation_ = glm::normalize(orientation_);

	//DO NOT remove the line below:
	viewMat = glm::lookAt(position, position + forwardVector, upVector);
	return 0;
}

int FPSCamera::Yaw(float angleDeg)
{
	
	glm::quat rotation = glm::angleAxis(glm::radians(angleDeg), WorldUp);
	orientation_ *= rotation;
	orientation_ = glm::normalize(orientation_);

	//DO NOT remove the line below:
	viewMat = glm::lookAt(position, position + forwardVector, upVector);
	return 0;
}
