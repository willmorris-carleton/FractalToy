/*
Description:
	Contains information regarding a scene which user can look at. Each scene has different,
	fractals and settings
*/
#ifndef SCENE_H_
#define SCENE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fpsCamera.h>
#include <GameTime.h>
#include <TextRenderer.h>

class Scene {

public:
	Scene(int id);
	void changeToScene(GLuint shader); //Updates sceneID and render settings to shader
	void updateUniforms(GLuint shader); //Updates sceneID and render settings to shader

	//DEFAULT CAMERA SETTINGS
	glm::vec3 startingPos = glm::vec3(0, 0, 4);
	glm::vec3 startingLookAt = glm::vec3(0, 0, 0);
	float movementSpeed = 20.f; //Units per second

	//AUTO CAMERA SETTINGS
	bool autoCameraOn = false;
	float cameraDistance = 10.f;
	float anglePerSecond = 10.f;
	glm::vec3 rotAxis = FPSCamera::WorldUp;


	//DEFAULT SCENE OPTIONS
	glm::vec3 backgroundColor = glm::vec3(0.53, 0.81, 0.92);
	glm::vec3 defaultObjectColor = glm::vec3(1, 0.9, 0.63);
	glm::vec3 ambient_color = glm::vec3(0.4, 0.4, 0.4);
	glm::vec3 diffuse_color = glm::vec3(0.75, 0.75, 0.75);
	glm::vec3 specular_color = glm::vec3(0.8, 0.8, 0.8);
	bool randomColorMode = false;

	//DEFAULT RENDER SETTINGS
	float renderDistance = 1000.f;

	bool fog = true;
	float fogStartDistance = 25.f;

	bool glow = true;
	float glowAmount = 0.1f; //Affects frequency of glow being added (number between 0 and 1)
	glm::vec3 glowColor = glm::vec3(1, 1, 1);

	bool shadows = true;
	bool ambientOcclusion = true;

	std::string sceneName = "empty name";

private:
	int sceneID; //ID used by shader to determine distance function
	void updateTextRendering();
	

};

#endif

