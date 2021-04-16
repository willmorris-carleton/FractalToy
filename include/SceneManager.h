/*
Description:
	Creates and changes between the different scenes the program has to offer
*/
#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <vector>
#include <Scene.h>

#define SCENE_INFINITE_SPHERES 0
#define SCENE_SIERPINSKY 1
#define SCENE_MENGER 2
#define SCENE_MANDELBROT 3


class SceneManager {

public:
	SceneManager(GLuint* shader);
	~SceneManager();
	void switchToScene(int sceneID);
	void updateUniforms();
	void changeScene(bool up);
	Scene* getCurrentScene();

private:
	std::vector<Scene*> scenes;
	int currentScene;
	GLuint* shader;


};

#endif

