/*
Description:
	Renders text
*/
#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <SOIL/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <fpsCamera.h>
#include <GameTime.h>
#include <Quad.h>

struct TextObj {
	std::string text;
	glm::vec3 color;
	glm::vec2 size;
};

class TextRenderer {

public:
	TextRenderer(GLuint* shader, Geometry* quad, GLuint* texture, Camera* cam);
	~TextRenderer();
	void RenderTexts();
	void addTextObj(TextObj* to);
	void clearTexts();

	static TextRenderer* tR;

private:
	float getGetXUV(char c);
	float getGetYUV(char c);

	std::vector<TextObj*> textObjs;
	GLuint* tshader;
	Geometry* quad;
	GLuint* textTexture;
	Camera* camera;

};

#endif

