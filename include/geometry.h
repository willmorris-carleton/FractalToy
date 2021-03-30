/*
Description:
	Contains the geometry vertex attributes for each shape.
*/
#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#define GLEW_STATIC

#include <GL/glew.h>

#include <glm/glm.hpp>

//define the Vertex struct containing position and color
struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;
};

//The geometry class contains the vertex attributes, the type
// and the name of the shape to create
class Geometry {

	public:
		Geometry() {}
		GLuint vbo; // OpenGL vertex buffer object
		GLuint ibo; // OpenGL index buffer object
		GLuint vao; //OpenGL vertex array object. Ignore this as its meant for Mac compatability and isn't important for the course. 
		GLuint size; // Size of data to be drawn

};

#endif

