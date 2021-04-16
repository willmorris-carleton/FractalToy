#include <TextRenderer.h>

TextRenderer* TextRenderer::tR = nullptr;

TextRenderer::TextRenderer(GLuint* shader, Geometry* quad, GLuint* texture, Camera* cam) : tshader(shader), quad(quad), textTexture(texture), camera(cam)
{
	tR = this;
}

TextRenderer::~TextRenderer()
{
	clearTexts();
}

void TextRenderer::RenderTexts()
{
	GLuint shader = *tshader;
	Geometry* geom = quad;

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

	//load the time uniform
	//GLint timer_var = glGetUniformLocation(shader, "time");
	//float time = glfwGetTime();
	//glUniform1f(timer_var, time);

	//Get the texture map uniform from the shader
	GLint tex = glGetUniformLocation(shader, "texture_map_text");

	// Assign the first texture to the map
	glUniform1i(tex, 0);
	glActiveTexture(GL_TEXTURE0);

	// First texture we bind
	glBindTexture(GL_TEXTURE_2D, *textTexture);

	// Define texture interpolation
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//define how the texture will be wrapped on the edges
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Render each text object
	for (int tIndex = 0; tIndex < textObjs.size(); tIndex++) {
		glm::vec3 scale = textObjs[tIndex].size * glm::vec3(0.10);
		std::string s = textObjs[tIndex].text;

		//Create the transform matrix
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(textObjs[tIndex].loc.x + scale.x , textObjs[tIndex].loc.y - scale.y, 0.5));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
		glm::mat4 transf = translationMatrix * scaleMatrix;

		for (int i = 0; i < s.length(); i++) {

			translationMatrix = glm::translate(translationMatrix, glm::vec3(scale.x / 1.15, 0, 0));
			transf = translationMatrix * scaleMatrix;

			GLint shader_mat = glGetUniformLocation(shader, "world_mat");
			glUniformMatrix4fv(shader_mat, 1, GL_FALSE, glm::value_ptr(transf));

			glm::mat4 view_matrix = camera->GetViewMatrix(NULL);
			glm::mat4 projection_matrix = camera->GetProjectionMatrix(NULL);

			shader_mat = glGetUniformLocation(shader, "view_mat");
			glUniformMatrix4fv(shader_mat, 1, GL_FALSE, glm::value_ptr(view_matrix));

			shader_mat = glGetUniformLocation(shader, "projection_mat");
			glUniformMatrix4fv(shader_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix));

			GLint textX = glGetUniformLocation(shader, "textX");
			glUniform1f(textX, getGetXUV(s[i]));

			GLint textY = glGetUniformLocation(shader, "textY");
			glUniform1f(textY, getGetYUV(s[i]));
 
			glDrawElements(GL_TRIANGLES, geom->size, GL_UNSIGNED_INT, 0);
		}
	}
	

	
}

void TextRenderer::addTextObj(TextObj to)
{
	textObjs.push_back(to);
}

void TextRenderer::clearTexts()
{
	textObjs.clear();
}


float TextRenderer::getGetXUV(char c)
{

	switch (c)
	{
	case '!':
	case ')':
	case '1':
	case '9':
	case 'a':
	case 'i':
	case 'q':
	case 'y':
		return 1;
		break;
	case '"':
	case '*':
	case '2':
	case ':':
	case 'b':
	case 'j':
	case 'r':
	case 'z':
		return 2;
		break;
	case '#':
	case '+':
	case '3':
	case ';':
	case 'c':
	case 'k':
	case 's':
	case '[':
		return 3;
		break;
	case '$':
	case ',':
	case '4':
	case '<':
	case 'd':
	case 'l':
	case 't':
	case '¥':
		return 4;
		break;
	case '%':
	case '-':
	case '5':
	case '=':
	case 'e':
	case 'm':
	case 'u':
	case ']':
		return 5;
		break;
	case ' ':
	case '.':
	case '6':
	case '>':
	case 'f':
	case 'n':
	case 'v':
	case '^':
		return 6;
		break;
	case '`':
	case '/':
	case '7':
	case '?':
	case 'g':
	case 'o':
	case 'w':
	case '_':
		return 7;
		break;
	default:
		return 0;
	}
}

float TextRenderer::getGetYUV(char c)
{

	switch (c)
	{
	case '(':
	case ')':
	case '*':
	case '+':
	case ',':
	case '-':
	case '.':
	case '/':
		return 1;
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		return 2;
		break;
	case '8':
	case '9':
	case ':':
	case ';':
	case '<':
	case '=':
	case '>':
	case '?':
		return 3;
		break;
	case '@':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
		return 4;
		break;
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
		return 5;
		break;
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
		return 6;
		break;
	case 'x':
	case 'y':
	case 'z':
	case '[':
	case '¥':
	case ']':
	case '^':
	case '_':
		return 7;
		break;
	default:
		return 0;
	}
}

