#include "pch.h"

#define DEBUG

#include "GPUProgram.h"
#include "Shader.h"
#include "Table.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glew.h>
#include <freeglut.h>
#include <vec2.hpp>
#include <vec3.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glm.hpp>

#include <iostream>
#include <vector>

#define ESC 27
#define SPACE 32
#define WIDTH 1440
#define HEIGHT 810


bool arrowHoldDown;

GLuint VAO;
GLuint VBO;
GPUProgram gpuProgram;
Shader shader;
Table table;

float tableV[] = {
	-300.0f, -300.0f, 0.5f,	// bal alul
	0.0f, 0.0f,
	300.0f, -300.0f, 0.5f,	// jobb alul
	1.0f, 0.0f,
	300.0f, 300.0f,	0.5f,	// jobb felül
	1.0f, 1.0f,
	-300.0f, 300.0f, 0.5,	// bal felül
	0.0f, 1.0f
};

unsigned int tableI[] = {0, 1, 2, 2, 3, 0};

/*float tableV[] = {
	-1.0f, -1.0f, 0.5f,	// bal alul
	0.0f, 0.0f,
	1.0f, -1.0f, 0.5f,	// jobb alul
	1.0f, 0.0f,
	1.0f, 1.0f,	0.5f,	// jobb felül
	1.0f, 1.0f,
	-1.0f, 1.0f, 0.5f,	// bal felül
	0.0f, 1.0f
};*/

#pragma region MiscellaneousHandlers
void onKeyDown(unsigned char c, int x, int y) noexcept
{
	if (c == ESC)
	{
		exit(0);
	}

	if (c == SPACE)
	{
		// UNDO mûvelet
	}
}

void onArrowDown(int key, int x, int y) noexcept
{
	if (!arrowHoldDown)
	{
		arrowHoldDown = true;

		if (key == GLUT_KEY_UP)
		{
			table.flip(Direction::UP);
			glutPostRedisplay();
		}

		if (key == GLUT_KEY_DOWN)
		{
			table.flip(Direction::DOWN);
			glutPostRedisplay();
		}

		if (key == GLUT_KEY_LEFT)
		{
			table.flip(Direction::LEFT);
			glutPostRedisplay();
		}

		if (key == GLUT_KEY_RIGHT)
		{
			table.flip(Direction::RIGHT);
			glutPostRedisplay();
		}

		#ifdef DEBUG
		std::cout << "----------------" << std::endl;
		table.print();
		#endif		
	}	
}

void onArrowRelease(int key, int x, int y) noexcept
{
	if (arrowHoldDown)
	{
		arrowHoldDown = false;
	}
}




void onKeyUp(unsigned char c, int x, int y)
{

}

void onMouse(int button, int state, int x, int y)
{

}

void onMouseMotion(int x, int y)
{

}
#pragma endregion

void onIdle()
{
	int time = glutGet(GLUT_ELAPSED_TIME);
}

void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, nullptr);


	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glutSwapBuffers();
}

void onInitialization()
{
	glViewport(0, 0, WIDTH, HEIGHT);
	table = std::move(Table(4));
	table.print();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint TBO;
	glGenTextures(1, &TBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TBO);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("C:/C++ graphics/2048/2048/textures/table.png", &width, &height, &nrChannels, 0);
	
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tableV), tableV, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	GLuint IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tableI), tableI, GL_STATIC_DRAW);

	gpuProgram.create(shader.VertexShader, shader.FragmentShader);

	unsigned int location;

	glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
	glm::mat4 V = glm::lookAt
	(
		glm::vec3(0.0f, 0.0f, 0.0f),	// camera position
		glm::vec3(0.0f, 0.0f, 1.0f),	// where we looking from
		glm::vec3(0.0f, 1.0f, 0.0f)		// up vector often (0,1,0), rarely (0, -1, 0) if we're upside down
	);
	glm::mat4 P = glm::ortho(-720.0f, +720.0f, -405.0f, +405.0f, -1.0f, +1.0f);
	glm::mat4 MVP = M * V * P;
	
	location = glGetUniformLocation(gpuProgram.getId(), "MVP");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(MVP));
	location = glGetUniformLocation(gpuProgram.getId(), "ourTexture");
	glUniform1i(location, 0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true);
	//unsigned char *data = stbi_load("C:/C++ graphics/9BallPool/9BallPool/textures/smiley.png", &width, &height, &nrChannels, 0);
	//unsigned int texture0, texture1;
	//
	//
	//glGenTextures(1, &texture0);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture0);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//
	//
	//
	//
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//
	//stbi_image_free(data);
	//
	//glGenTextures(1, &texture1);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture1);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//
	//
	//data = stbi_load("C:/C++ graphics/9BallPool/9BallPool/textures/box.jpg", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//
	//stbi_image_free(data);
	//
	//
	//glGenBuffers(1, &VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
	//
	//
	//GLuint IBO;
	//glGenBuffers(1, &IBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	//
	//
	//
	//
	//

	
	//
	//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -5.0));
	//
	//glm::mat4 viewMtx = glm::lookAt
	//(
	//	glm::vec3(0.0f, 2.0f, 0.0f),	// camera position
	//	glm::vec3(0.0f, 0.0f, -5.0f),	// where we looking at
	//	glm::vec3(0.0f, 1.0f, 0.0f)		// up vector often (0,1,0), rarely (0, -1, 0) if we're upside down
	//);
	//
	//glm::mat4 projMtx = glm::perspective(glm::radians(45.0f), 1.0f * WIDTH / HEIGHT, 0.1f, 10.0f);
	////glm::mat4 projMtx = glm::ortho(-2.0f, 2.0f, -2.0f, +2.0f, -1.0f, 1.0f);
	//
	//
	//glm::mat4 mvp = projMtx * viewMtx * model;
	//unsigned int location = glGetUniformLocation(gpuProgram.getId(), "M");
	//glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvp));
	//
	//
	//
	//
	//
	//location = glGetUniformLocation(gpuProgram.getId(), "ourTexture");
	//glUniform1i(location, 0);
	//location = glGetUniformLocation(gpuProgram.getId(), "ourTexture2");
	//glUniform1i(location, 1);
	//
	//
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void APIENTRY debugCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	const char* _source;
	const char* _type;
	const char* _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "UNKNOWN";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "NOTIFICATION";
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

	printf("%d:\n\n%s of %s severity, raised from %s:\n\n%s\n", id, _type, _severity, _source, message);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL 4.3 2048");

	glutInitContextVersion(4, 3);
	glewExperimental = true;
	GLenum initResults = glewInit();
	if (initResults != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}
	glEnable(GL_DEPTH_TEST);


	// enable instant error throw
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debugCallbackFunction, nullptr);



	// full nulla Z-knél megfordítja a rajzolási irányt. A késõbb jövõk nem mennek át a depth testen, azok lesznek alul.
	onInitialization();

	glutDisplayFunc(onDisplay);
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutMotionFunc(onMouseMotion);
	glutSpecialFunc(onArrowDown);		// keyboard inputs that aren't in the range of unsigned char
	glutSpecialUpFunc(onArrowRelease);

	glutMainLoop();
	exit(EXIT_SUCCESS);

	return 0;
}