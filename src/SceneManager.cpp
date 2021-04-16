#include <SceneManager.h>

SceneManager::SceneManager(GLuint* shader) : shader(shader), currentScene(0)
{
	//Create scenes
	Scene* spheres = new Scene(SCENE_INFINITE_SPHERES);
	spheres->movementSpeed = 20.f;
	spheres->renderDistance = 200.f;
	spheres->shadows = false;
	spheres->glow = false;
	spheres->defaultObjectColor = glm::vec3(0, .5, 0);
	spheres->randomColorMode = true;
	spheres->sceneName = "spheres";
	scenes.push_back(spheres);

	Scene* sierpinsky = new Scene(SCENE_SIERPINSKY);
	sierpinsky->movementSpeed = 200.f;
	sierpinsky->renderDistance = 1800.f;
	sierpinsky->autoCameraOn = true;
	sierpinsky->startingPos = glm::vec3(5,0,5);
	sierpinsky->startingLookAt = glm::vec3(0,0,0);
	sierpinsky->cameraDistance = 1200.f;
	sierpinsky->backgroundColor = glm::vec3(0.2,0.6,0.2);
	sierpinsky->sceneName = "sierpinsky";
	scenes.push_back(sierpinsky);

	Scene* menger = new Scene(SCENE_MENGER);
	menger->movementSpeed = 30;
	menger->startingPos = glm::vec3(200,50,200);
	menger->startingLookAt = glm::vec3(0,125,0);
	menger->backgroundColor = glm::vec3(0.3,0,0);
	menger->defaultObjectColor = glm::vec3(0.7,0,0);
	menger->ambient_color = glm::vec3(0.6,0.6,0.6);
	menger->sceneName = "menger sponge";
	scenes.push_back(menger);

	Scene* mandel = new Scene(SCENE_MANDELBROT);
	mandel->movementSpeed = 1.5f;
	mandel->autoCameraOn = true;
	mandel->startingPos = glm::vec3(0, 0, 1);
	mandel->startingLookAt = glm::vec3(0, 0, 0);
	mandel->cameraDistance = 2.5f;
	mandel->backgroundColor = glm::vec3(0, 0, 0);
	mandel->defaultObjectColor = glm::vec3(0.4, 0.2, 0.6);
	mandel->specular_color = glm::vec3(0.1);
	mandel->sceneName = "mandelbrot";
	scenes.push_back(mandel);
}

SceneManager::~SceneManager()
{
	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}
}

void SceneManager::switchToScene(int sceneID)
{
	if (sceneID > scenes.size()) return;
	currentScene = sceneID;
	scenes[sceneID]->changeToScene(*shader);
}

void SceneManager::updateUniforms()
{
	scenes[currentScene]->updateUniforms(*shader);
}

void SceneManager::changeScene(bool up)
{
	int i = currentScene + (up ? 1 : -1);
	if ((up && i < scenes.size()) || (!up && i >= 0)) {
		switchToScene(i);
	}
}

Scene* SceneManager::getCurrentScene()
{
	return scenes[currentScene];
}

