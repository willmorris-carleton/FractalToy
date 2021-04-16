/*
Description:
	Program that allows users to manipulate and customize fractals
	to create visually pleasant for users to use as wallpapers/screensavers
Copyright (c):
	2021 William Morris
*/


#include <iostream>
#include <stdexcept>
#include <string>
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

#include <camera.h>
#include <fpsCamera.h>
#include <geometry.h>
#include <quad.h>
#include <GameTime.h>
#include <GameInput.h>
#include <SceneManager.h>
#include <TextRenderer.h>

#include <fstream>
#include <sstream>
#include <vector>

//Change this file path to point to the resource/ directory containing the shaders. 
#define DIRECTORY "C:/Users/willi/Desktop/Graphics/project/FractalToy/"

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

// Globals that define the OpenGL window and viewport
const std::string window_title_g = "COMP3009: Final Project";
const unsigned int window_width_g = 1600;
const unsigned int window_height_g = 1200;
const glm::vec3 background(0.0, 0.0, 0.0);

// Globals that define the OpenGL camera view and projection
glm::vec3 camera_position_g(0, 0, 3.0); // Position of camera
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0); // Point looking at
glm::vec3 camera_up_g(0.0, 1.0, 0.0); // Up vector
float camera_near_clip_distance_g = 0.01; // Near clipping plane
float camera_far_clip_distance_g = 1000.0; // Far clipping plane
float camera_fov_g = 60.0; // Field-of-view for projection

//triangle array of vertices
std::vector<Vertex>  vertices;
std::vector<int>  indices;

//matricies
glm::mat4 view_matrix, projection_matrix;

GLFWwindow* window;
Camera* camera;
Camera* rayCamera;
glm::vec3 light_pos;

GameTime* gTime;
GameInput* gInput;
SceneManager* sm;
TextRenderer* textRenderer;

//This function is used to read the shader programs. OpenGL does not read them as a
//specific file type, but instead as simply a text file containing c code. 
std::string LoadTextFile(std::string filename) {

	const char* char_file = filename.c_str();
	std::ifstream f;
	f.open(char_file);
	if (f.fail()) {
		throw(std::ios_base::failure(std::string("Error opening file ") + std::string(filename)));
	}

	std::string content;
	std::string line;
	while (std::getline(f, line)) {
		content += line + "\n";
	}

	f.close();

	return content;
}

//This function loads the fragment and vertex shader. 
GLuint LoadShaders(std::string shaderName) {

	// Create a shader from vertex program source code
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	
	//Read in the shaders into strings
	std::string fragment_shader = LoadTextFile(std::string(DIRECTORY) + "resource/"+shaderName +".frag");
	std::string vertex_shader = LoadTextFile(std::string(DIRECTORY) +"resource/" +shaderName +".vert");

	//Compile the vertex shader from the source string
	const char* const_vertex_shader = vertex_shader.c_str();
	glShaderSource(vs, 1, &const_vertex_shader, NULL);
	glCompileShader(vs);

	// Check if shader compiled successfully
	GLint status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vs, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error compiling vertex shader: ") + std::string(buffer)));
	}

	//Compile the fragment shader from the source string
	const char* const_fragment_shader = fragment_shader.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &const_fragment_shader, NULL);
	glCompileShader(fs);

	// Check if shader compiled successfully
	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fs, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error compiling fragment shader: ") + std::string(buffer)));
	}

	// Create a shader program linking both vertex and fragment shaders together
	GLuint shader = glCreateProgram();
	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	// Check if shaders were linked successfully
	glGetProgramiv(shader, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error linking shaders: ") + std::string(buffer)));
	}

	// Delete memory used by shaders, since they were already compiled
	// and linked
	glDeleteShader(vs);
	glDeleteShader(fs);

	return shader;

}

float testvarA = 0.0f;

// Callback for when a key is pressed
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	gInput->keyCallback(window, key, scancode, action, mods);

	//Only when key is pressed check these
	if (gInput->getKey(GLFW_KEY_UP)) {
		sm->changeScene(true);
	}
	if (gInput->getKey(GLFW_KEY_DOWN)) {
		sm->changeScene(false);
	}
	if (gInput->getKey(GLFW_KEY_O)) {
		testvarA -= 1;
		std::cout << testvarA << std::endl;
	}
	if (gInput->getKey(GLFW_KEY_P)) {
		testvarA += 1;
		std::cout << testvarA << std::endl;
	}
}


//Checked every frame
void applyKeyboardInput() {

	// Quit the program when pressing 'q'
	if (gInput->getKey(GLFW_KEY_Q)) {
		glfwSetWindowShouldClose(window, true);
	}

	// View control
	float rot_factor(glm::pi<float>() / 180);
	float trans_factor = FPSCamera::movementSpeed * gTime->getDelta(); //0.25 units per second
	rot_factor *= 200;

	if (gInput->getKey(GLFW_KEY_LEFT)) {
		//rayCamera->Yaw(-rot_factor);
	}
	if (gInput->getKey(GLFW_KEY_RIGHT)) {
		//rayCamera->Yaw(rot_factor);
	}
	if (gInput->getKey(GLFW_KEY_N)) {
		//rayCamera->Roll(-rot_factor);
	}
	if (gInput->getKey(GLFW_KEY_M)) {
		//rayCamera->Roll(rot_factor);
	}
	if (gInput->getKey(GLFW_KEY_W)) {
		rayCamera->MoveForward(trans_factor);
	}
	if (gInput->getKey(GLFW_KEY_S)) {
		rayCamera->MoveBackward(trans_factor);
	}
	if (gInput->getKey(GLFW_KEY_A)) {
		rayCamera->moveLeft(-trans_factor);
	}
	if (gInput->getKey(GLFW_KEY_D)) {
		rayCamera->MoveRight(-trans_factor);
	}
	if (gInput->getKey(GLFW_KEY_SPACE)) {
		rayCamera->MoveUp(-trans_factor);
	}
	if (gInput->getKey(GLFW_KEY_LEFT_CONTROL)) {
		rayCamera->MoveDown(-trans_factor);
	}

	if (gInput->getKey(GLFW_KEY_H)) {
		sm->switchToScene(SCENE_INFINITE_SPHERES);
	}
	if (gInput->getKey(GLFW_KEY_J)) {
		sm->switchToScene(SCENE_SIERPINSKY);
	}

}

const float mouseSensitivity = 15.f; //Degrees per second
bool focused = false;

void focusedCallback(GLFWwindow* window, int focused) {
	if (focused == GLFW_FOCUSED) focused = true;
	else focused = false;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (focused == false) {
		glfwSetCursorPos(window, window_width_g / 2, window_height_g / 2);
		focused = true;
		return;
	}

	float middleX = window_width_g / 2;
	float middleY = window_height_g / 2;
	
	rayCamera->Yaw((middleX - xpos) * mouseSensitivity * gTime->getDelta());
	rayCamera->Pitch((middleY - ypos) * mouseSensitivity * gTime->getDelta());

	glfwSetCursorPos(window, window_width_g / 2, window_height_g / 2);
}


// Callback for when the window is resized
void ResizeCallback(GLFWwindow* window, int width, int height) {

	// Set OpenGL viewport based on framebuffer width and height
	glViewport(0, 0, width, height);

	// Update projection matrix
	void* ptr = glfwGetWindowUserPointer(window);
	glm::mat4 *projection_matrix = (glm::mat4 *) ptr;
	float top = tan((camera_fov_g / 2.0)*(glm::pi<float>() / 180.0))*camera_near_clip_distance_g;
	float right = top * width / height;
	(*projection_matrix) = glm::frustum(-right, right, -top, top, camera_near_clip_distance_g, camera_far_clip_distance_g);
}

GLuint LoadTexture(std::string filename) {

	// Create texture string name
	std::string texture_name = std::string(DIRECTORY) + "assets/" + filename;

	//Load the texture using the SOIL library, and create a new ID for it
	GLuint texture = SOIL_load_OGL_texture(texture_name.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	
	//error handling in case the texture wasn't loaded
	if (!texture) {
		throw(std::ios_base::failure(std::string("Error loading texture ") + std::string(texture_name) + std::string(": ") + std::string(SOIL_last_result())));
	}

	// Create resource
	return texture;
}


//This function sends the matrix into the uniform in the shader 
//uniforms are attributes that are defined in the shader but can be
//accessed outside it. 
void LoadShaderMatrix(GLuint shader, glm::mat4 matrix, std::string name) {

	//First get the uniform from the shader by specifying its name
	GLint shader_mat = glGetUniformLocation(shader, name.c_str());

	//Now load the matrix using the proper uniform (matrix4fv) function. 
	glUniformMatrix4fv(shader_mat, 1, GL_FALSE, glm::value_ptr(matrix));
}

//print opengl info
void PrintOpenGLInfo(void) {

	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

}

void Render(Geometry* geom, GLuint shader,glm::vec3 translation, glm::vec3 scale, glm::quat orientation, GLuint texture = 0, GLuint texture2 = 0) {

	//Tell OpenGL to use the specific shader
	glUseProgram(shader);

	//Bind the VAO
	glBindVertexArray(geom->vao);

	//Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, geom->vbo);

	//Bind the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->ibo);

	//Load the vertex attributes from the VBO into the shader

	//First access the "vertex" from the shader
	GLint vertex_att = glGetAttribLocation(shader, "vertex");

	//This function works simiarly to the uniform and doesn't need to be used for verticies. 
	//The 2nd parameter is the number of values being selected, the 2nd last parameter
	//is the size of each attribute, and the last is the memory offset location (In case
	//you don't want to read from the begining). 
	glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(vertex_att);

	GLint color_att = glGetAttribLocation(shader, "color");
	glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
	glEnableVertexAttribArray(color_att);

	GLint normal_att = glGetAttribLocation(shader, "normal");
	glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
	glEnableVertexAttribArray(normal_att);

	GLint uv_att = glGetAttribLocation(shader, "uv");
	glVertexAttribPointer(uv_att, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)36);
	glEnableVertexAttribArray(uv_att);

	//Create the transform matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rotationMatrix = glm::mat4_cast(orientation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 transf = translationMatrix * rotationMatrix * scaleMatrix;

	//Load all the shader matricies into thier uniforms
	LoadShaderMatrix(shader,transf, "world_mat");

	view_matrix = camera->GetViewMatrix(NULL);
	projection_matrix = camera->GetProjectionMatrix(NULL);

	LoadShaderMatrix(shader,view_matrix, "view_mat");
	LoadShaderMatrix(shader,projection_matrix, "projection_mat");

	//load the time uniform
	GLint timer_var = glGetUniformLocation(shader, "time");
	float time = glfwGetTime();
	glUniform1f(timer_var, time);

	//load camera position
	GLint cam_var = glGetUniformLocation(shader, "camPos");
	glUniform3fv(cam_var, 1, glm::value_ptr(camera->GetPosition()));

	//load the time uniform
	GLint uLightPos = glGetUniformLocation(shader, "lightPos");
	glUniform3fv(uLightPos, 1, glm::value_ptr(light_pos));

	//if the texture exists, need to load it
	if (texture != 0) {

		//Get the texture map uniform from the shader
		GLint tex = glGetUniformLocation(shader, "texture_map");

		// Assign the first texture to the map
		glUniform1i(tex, 0); 
		glActiveTexture(GL_TEXTURE0);

		// First texture we bind
		glBindTexture(GL_TEXTURE_2D, texture); 

		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//define how the texture will be wrapped on the edges
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (texture2 != 0) {
			//Get the texture map uniform from the shader
			GLint tex2 = glGetUniformLocation(shader, "second_texture_map");

			// Assign the first texture to the map
			glUniform1i(tex2, 1);
			glActiveTexture(GL_TEXTURE1);

			// First texture we bind
			glBindTexture(GL_TEXTURE_2D, texture2);

			// Define texture interpolation
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//define how the texture will be wrapped on the edges
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	}
	else {
		glBindTexture(shader, 0);
	}

	//Finally, draw using GL_TRIANGLES, which reads 3 at a time until it reaches the total size 
	glDrawElements(GL_TRIANGLES, geom->size, GL_UNSIGNED_INT, 0);

}

void RenderQuadScreen(Geometry* geom, GLuint shader, glm::vec3 translation, glm::vec3 scale, glm::quat orientation) {

	//Tell OpenGL to use the specific shader
	glUseProgram(shader);

	//Bind the VAO
	glBindVertexArray(geom->vao);

	//Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, geom->vbo);

	//Bind the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->ibo);

	//Load the vertex attributes from the VBO into the shader

	//First access the "vertex" from the shader
	GLint vertex_att = glGetAttribLocation(shader, "vertex");

	//This function works simiarly to the uniform and doesn't need to be used for verticies. 
	//The 2nd parameter is the number of values being selected, the 2nd last parameter
	//is the size of each attribute, and the last is the memory offset location (In case
	//you don't want to read from the begining). 
	glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(vertex_att);

	GLint color_att = glGetAttribLocation(shader, "color");
	glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
	glEnableVertexAttribArray(color_att);

	GLint normal_att = glGetAttribLocation(shader, "normal");
	glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
	glEnableVertexAttribArray(normal_att);

	GLint uv_att = glGetAttribLocation(shader, "uv");
	glVertexAttribPointer(uv_att, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)36);
	glEnableVertexAttribArray(uv_att);

	view_matrix = camera->GetViewMatrix(NULL);
	projection_matrix = camera->GetProjectionMatrix(NULL);

	//Update location of quad based off camera location
	float zvp = 3.75*(1 / projection_matrix[2][3]);
	//std::cout << "ZVP: " << zvp << std::endl;
	translation = camera->GetPosition() - (camera->GetForward() * (zvp));
	//std::cout << "translation: " << translation.x << " " << translation.y << " " << translation.z << std::endl;

	//Create the transform matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rotationMatrix = glm::mat4_cast(orientation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 transf = translationMatrix * rotationMatrix; //Screen is simply a transformation of camera's view matrix

	//Load all the shader matricies into thier uniforms
	LoadShaderMatrix(shader, transf, "world_mat");

	LoadShaderMatrix(shader, view_matrix, "view_mat");
	LoadShaderMatrix(shader, projection_matrix, "projection_mat");

	//load the time uniform
	GLint timer_var = glGetUniformLocation(shader, "time");
	float time = glfwGetTime();
	glUniform1f(timer_var, time);

	GLint tester_var = glGetUniformLocation(shader, "testvarA");
	glUniform1f(tester_var, testvarA);

	//load camera position
	GLint cam_var = glGetUniformLocation(shader, "camPos");
	glUniform3fv(cam_var, 1, glm::value_ptr(rayCamera->GetPosition()));

	GLint cam1_var = glGetUniformLocation(shader, "cameraUp");
	glUniform3fv(cam1_var, 1, glm::value_ptr(rayCamera->GetUp()));

	GLint cam2_var = glGetUniformLocation(shader, "cameraLookAt");
	glUniform3fv(cam2_var, 1, glm::value_ptr(rayCamera->GetLookAtPoint()));

	//std::cout << "cam: " << camPos.x << " " << camPos.y << " " << camPos.z << std::endl;

	//load the time uniform
	GLint uLightPos = glGetUniformLocation(shader, "lightPos");
	glUniform3fv(uLightPos, 1, glm::value_ptr(light_pos));

	//Finally, draw using GL_TRIANGLES, which reads 3 at a time until it reaches the total size 
	glDrawElements(GL_TRIANGLES, geom->size, GL_UNSIGNED_INT, 0);

}

// Main function that builds and runs the application
int main(void) {

	try {
		// Initialize the window management library (GLFW)
		if (!glfwInit()) {
			throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
		}

		//Mac uses an older version of OpenGL. So we need tos et it to use
		//the neweer core profile. If you're on windows, remove this. 
		#ifdef __APPLE__
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
		#endif

		//Basic Anti Aliasing (Comment out for better performance)
		glfwWindowHint(GLFW_SAMPLES, 4);
		glEnable(GL_MULTISAMPLE);

		// Create a window and its OpenGL context
		window = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
		if (!window) {
			glfwTerminate();
			throw(std::runtime_error(std::string("Could not create window")));
		}

		// Make the window's OpenGL context the current one
		glfwMakeContextCurrent(window);

		// Initialize the GLEW library to access OpenGL extensions
		// Need to do it after initializing an OpenGL context
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_CULL_FACE);

		//Camera for rendering quad and text
		camera = new Camera();
		camera->SetCamera(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		//Camera for raymarching
		rayCamera = (Camera* ) new FPSCamera();
		rayCamera->SetCamera(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		
		//Load the shaders
		GLuint marchShader = LoadShaders("rayMarchShader");
		GLuint textShader = LoadShaders("textShader");

		sm = new SceneManager(&marchShader);
		
		// Create screen
		Geometry* square = (Geometry*) new Quad();
		GLuint textTexture = LoadTexture("FontSheet.bmp");

		gInput = new GameInput();
		textRenderer = new TextRenderer(&textShader, square, &textTexture, camera);

		//Set the proper callbacks. 
		glfwSetWindowUserPointer(window, (void *)&projection_matrix);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetFramebufferSizeCallback(window, ResizeCallback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouseCallback);
		

		//Define the three transformation attributes
		glm::quat orientation = glm::angleAxis(0.0f, glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 scale = glm::vec3(1);
		glm::vec3 translation = glm::vec3(0.0);
		light_pos = glm::vec3(1, -25, 4);

		//Initialize time
		gTime = new GameTime();

		//Set start scene
		sm->switchToScene(SCENE_INFINITE_SPHERES);

		PrintOpenGLInfo();

		while (!glfwWindowShouldClose(window)) {

			gTime->update();
			
			//Clear the screen
			glClearColor(background[0],background[1],background[2], 0.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Render the texts
			textRenderer->RenderTexts();
			
			//light_pos = rayCamera->GetPosition();
			RenderQuadScreen(square, marchShader, translation, scale, orientation);
			sm->updateUniforms();

			glfwPollEvents();
			applyKeyboardInput();
			glfwSwapBuffers(window);

			std::cout << "FPS: " << GameTime::gt->getFPS() << std::endl;
		}
	}
	catch (std::exception &e) {
		PrintException(e);
	}

	return 0;
}
