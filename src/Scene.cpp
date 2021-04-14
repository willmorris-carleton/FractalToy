#include <Scene.h>
#include <iostream>

Scene::Scene(int id) : sceneID(id) {}

void Scene::changeToScene(GLuint shader)
{

	//DEFAULT CAMERA SETTINGS
	if (!autoCameraOn) {
		FPSCamera::currentCam->SetCamera(startingPos, startingLookAt, FPSCamera::WorldUp);
		FPSCamera::movementSpeed = movementSpeed;
	}
	else {
		FPSCamera::currentCam->SetCamera(glm::normalize(startingPos)* cameraDistance, startingPos, FPSCamera::WorldUp);
		FPSCamera::movementSpeed = movementSpeed;
	}
	

	updateUniforms(shader);

}

void Scene::updateUniforms(GLuint shader)
{
	if (autoCameraOn) {
		float distanceFromOrigin = glm::length(FPSCamera::currentCam->GetPosition());
		FPSCamera::currentCam->MoveForward(distanceFromOrigin);
		FPSCamera::currentCam->RotateAroundAxis(anglePerSecond*GameTime::gt->getDelta(), rotAxis);
		FPSCamera::currentCam->MoveBackward(distanceFromOrigin);

		//std::cout << anglePerSecond * GameTime::gt->getDelta() << std::endl;
	}

	//DEFAULT SCENE OPTIONS
	GLint u_var = glGetUniformLocation(shader, "bgColor");
	glUniform3fv(u_var, 1, glm::value_ptr(backgroundColor));

	u_var = glGetUniformLocation(shader, "objectHitCol");
	if (randomColorMode) {
		glm::vec3 s = glm::vec3(cos(glfwGetTime()) + 1, sin(glfwGetTime()) + 1, -sin(glfwGetTime()) + 1);
		glUniform3fv(u_var, 1, glm::value_ptr(s));
	}
	else {
		glUniform3fv(u_var, 1, glm::value_ptr(defaultObjectColor));
	}

	u_var = glGetUniformLocation(shader, "ambient_color");
	glUniform3fv(u_var, 1, glm::value_ptr(ambient_color));

	u_var = glGetUniformLocation(shader, "diffuse_color");
	glUniform3fv(u_var, 1, glm::value_ptr(diffuse_color));

	u_var = glGetUniformLocation(shader, "specular_color");
	glUniform3fv(u_var, 1, glm::value_ptr(specular_color));

	//RENDER SETTINGS
	u_var = glGetUniformLocation(shader, "maxDistance");
	glUniform1f(u_var, renderDistance);

	u_var = glGetUniformLocation(shader, "FOG_ENABLED");
	glUniform1i(u_var, fog);

	u_var = glGetUniformLocation(shader, "minFogDistance");
	glUniform1f(u_var, fogStartDistance);

	u_var = glGetUniformLocation(shader, "GLOW_ENABLED");
	glUniform1i(u_var, glow);

	u_var = glGetUniformLocation(shader, "glowSteps");
	glUniform1i(u_var, (int)(glowAmount * 100));

	u_var = glGetUniformLocation(shader, "glow");
	glUniform3fv(u_var, 1, glm::value_ptr(glowColor));

	u_var = glGetUniformLocation(shader, "SHADOWS_ENABLED");
	glUniform1i(u_var, shadows);

	u_var = glGetUniformLocation(shader, "sceneID");
	glUniform1i(u_var, sceneID);

}


