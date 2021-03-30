#include <quad.h>

Quad::Quad() : Geometry()
{
	//triangle array of vertices
	std::vector<Vertex>  vertices;
	std::vector<int>  indices;

	vertices.resize(4);
	indices.resize(6);

	vertices[0].pos = glm::vec3(-1, -1, 0);
	vertices[1].pos = glm::vec3(-1, 1, 0);
	vertices[2].pos = glm::vec3(1, 1, 0);
	vertices[3].pos = glm::vec3(1, -1, 0);

	vertices[0].normal = glm::vec3(0, 0, 1);
	vertices[1].normal = glm::vec3(0, 0, 1);
	vertices[2].normal = glm::vec3(0, 0, 1);
	vertices[3].normal = glm::vec3(0, 0, 1);

	vertices[0].color = glm::vec3(1, 1, 1);
	vertices[1].color = glm::vec3(0, 0, 1);
	vertices[2].color = glm::vec3(0, 1, 0);
	vertices[3].color = glm::vec3(1, 0, 0);

	vertices[0].uv = glm::vec2(0, 1);
	vertices[1].uv = glm::vec2(0, 0);
	vertices[2].uv = glm::vec2(1, 0);
	vertices[3].uv = glm::vec2(1, 1);

	int arr[] = {
		0, 2, 1,
		0, 3, 2
	};

	indices.assign(arr, arr + 6);

	GLuint face[] = { 0, 2, 1,
					 0, 3, 2 };

	// Create geometry
	Quad* geom = this;
	size = 6;

	glGenVertexArrays(1, &geom->vao);
	glBindVertexArray(geom->vao);

	// Create OpenGL buffer for vertices
	glGenBuffers(1, &geom->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, geom->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &vertices[0], GL_STATIC_DRAW);

	// Create OpenGL buffer for faces
	glGenBuffers(1, &geom->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), &indices[0], GL_STATIC_DRAW);

	// Free data buffers
	vertices.clear();
	indices.clear();
}
