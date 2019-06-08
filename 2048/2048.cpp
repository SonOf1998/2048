#include "pch.h"

//#define DEBUG

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
#include <string>
#include <vector>

#include <mmsystem.h>
#include <Windows.h>

#define ESC 27
#define SPACE 32
#define WIDTH 1440
#define HEIGHT 810

bool arrowHoldDown;
bool firstRun;
bool undoUsed;

int distinctElementCnt = 0;

GLuint VAO1, VAO2;
GLuint TBO1, TBO2;

std::vector<GLuint> VAO;
std::vector<GLuint> TBO;
std::vector<int> numOfTris;

GPUProgram gpuProgram;
Shader shader;
Table table;
Memento *memento = nullptr;


void fillTable()
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	VAO.clear();
	TBO.clear();
	numOfTris.clear();

	int i = 0;
	std::set<int> uniqueElems = table.getUniqueElements();
	size_t s = uniqueElems.size();

	VAO.reserve(s);
	TBO.reserve(s);
	numOfTris.reserve(s);

	for (int elem : uniqueElems)
	{
		VAO.emplace_back();
		TBO.emplace_back();

		std::vector<float> tileV = std::move(table.getVertexData(elem));
		std::vector<unsigned int> tileI = std::move(table.getIndexData(elem));

		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);

		glGenTextures(1, &TBO[i]);
		glBindTexture(GL_TEXTURE_2D, TBO[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::string pathToTexture = "C:/C++ graphics/2048/2048/textures/tile_" + std::to_string(elem) + ".png";
		unsigned char *data = stbi_load(pathToTexture.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tileV.size(), &tileV[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

		GLuint IBO;
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tileI.size(), &tileI[0], GL_STATIC_DRAW);

		numOfTris.push_back(tileI.size());
		distinctElementCnt = ++i;
	}
}

#pragma region MiscellaneousHandlers
void onKeyDown(unsigned char c, int x, int y) noexcept
{
	if (c == ESC)
	{
		if (memento != nullptr)
		{
			delete memento;
		}
		exit(0);
	}

	if (c == SPACE && !undoUsed && !firstRun)
	{
		table.restore(memento);
		fillTable();
		glutPostRedisplay();
		undoUsed = true;
		PlaySound(TEXT("C:\\C++ graphics\\2048\\2048\\sound\\revert.wav"), NULL, SND_ASYNC | SND_FILENAME);
	}
}

void manageLastState()
{
	if (memento != nullptr)
	{
		delete memento;
	}

	memento = table.createMemento();
}

void onArrowDown(int key, int x, int y) noexcept
{
	if (!arrowHoldDown)
	{
		if (key == GLUT_KEY_UP && table.flipAllowed(Direction::UP))
		{
			PlaySound(TEXT("C:\\C++ graphics\\2048\\2048\\sound\\swipe.wav"), NULL, SND_ASYNC | SND_FILENAME);
			arrowHoldDown = true;
			undoUsed = false;
			firstRun = false;
			manageLastState();
			table.flip(Direction::UP);
		} 
		else if (key == GLUT_KEY_DOWN && table.flipAllowed(Direction::DOWN))
		{
			PlaySound(TEXT("C:\\C++ graphics\\2048\\2048\\sound\\swipe.wav"), NULL, SND_ASYNC | SND_FILENAME);
			arrowHoldDown = true;
			undoUsed = false;
			firstRun = false;
			manageLastState();
			table.flip(Direction::DOWN);
		} 
		else if (key == GLUT_KEY_LEFT && table.flipAllowed(Direction::LEFT))
		{
			PlaySound(TEXT("C:\\C++ graphics\\2048\\2048\\sound\\swipe.wav"), NULL, SND_ASYNC | SND_FILENAME);
			arrowHoldDown = true;
			undoUsed = false;
			firstRun = false;
			manageLastState();
			table.flip(Direction::LEFT);
		}
		else if (key == GLUT_KEY_RIGHT && table.flipAllowed(Direction::RIGHT))
		{
			PlaySound(TEXT("C:\\C++ graphics\\2048\\2048\\sound\\swipe.wav"), NULL, SND_ASYNC | SND_FILENAME);
			arrowHoldDown = true;
			undoUsed = false;
			firstRun = false;
			manageLastState();
			table.flip(Direction::RIGHT);
		} 
		else
		{
			PlaySound(TEXT("C:\\C++ graphics\\2048\\2048\\sound\\error.wav"), NULL, SND_ASYNC | SND_FILENAME);
			return;
		}

		fillTable();
		glutPostRedisplay();

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
	glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TBO1);
	glUniform1i(glGetUniformLocation(gpuProgram.getId(), "smp"), 0);
	glBindVertexArray(VAO1);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, nullptr);

	if (firstRun)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TBO2);
		glUniform1i(glGetUniformLocation(gpuProgram.getId(), "smp"), 0);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, nullptr);
	}

	for (int i = 0; i < distinctElementCnt; ++i)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TBO[i]);
		glUniform1i(glGetUniformLocation(gpuProgram.getId(), "smp"), 0);
		glBindVertexArray(VAO[i]);
		glDrawElements(GL_TRIANGLES, numOfTris[i] * 3, GL_UNSIGNED_INT, nullptr);
	}
	

	glutSwapBuffers();
}

void sendDefaultTableDataToGPU()
{
	float tableV[] = {
	-310.0f, -310.0f, 0.5f,	// bal alul
	0.0f, 0.0f,
	310.0f, -310.0f, 0.5f,	// jobb alul
	1.0f, 0.0f,
	310.0f, 310.0f,	0.5f,	// jobb felül
	1.0f, 1.0f,
	-310.0f, 310.0f, 0.5,	// bal felül
	0.0f, 1.0f
	};

	unsigned int tableI[] = { 0, 1, 2, 2, 3, 0 };

	glGenVertexArrays(1, &VAO1);
	glBindVertexArray(VAO1);

	glGenTextures(1, &TBO1);
	glBindTexture(GL_TEXTURE_2D, TBO1);

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

	GLuint VBO1;
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tableV), tableV, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	GLuint IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tableI), tableI, GL_STATIC_DRAW);



	// a táblán alapból elhelyezkedõ két kettes négyzet..
	std::vector<float> tileV = std::move(table.getVertexData(2));
	std::vector<unsigned int> tileI = std::move(table.getIndexData(2));
	
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);

	glGenTextures(1, &TBO2);
	glBindTexture(GL_TEXTURE_2D, TBO2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("C:/C++ graphics/2048/2048/textures/tile_2.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	GLuint VBO2;
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tileV.size(), &tileV[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

	GLuint IBO2;
	glGenBuffers(1, &IBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tileI.size(), &tileI[0], GL_STATIC_DRAW);
}

void onInitialization()
{
	glViewport(0, 0, WIDTH, HEIGHT);
	table = std::move(Table(4));
#ifdef DEBUG
	table.print();
#endif



	sendDefaultTableDataToGPU();

	gpuProgram.create(shader.VertexShader, shader.FragmentShader);

	unsigned int location;

	glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
	glm::mat4 V = glm::lookAt
	(
		glm::vec3(0.0f, 0.0f, 0.0f),	// camera position
		glm::vec3(0.0f, 0.0f, -1.0f),	// where we looking from
		glm::vec3(0.0f, 1.0f, 0.0f)		// up vector often (0,1,0), rarely (0, -1, 0) if we're upside down
	);
	glm::mat4 P = glm::ortho(-720.0f, +720.0f, -405.0f, +405.0f, -1.0f, +1.0f);
	glm::mat4 MVP = M * V * P;
	
	location = glGetUniformLocation(gpuProgram.getId(), "MVP");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(MVP));


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	firstRun = true;

	memento = table.createMemento();
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
		_type = "UNDEFINED BEHAVIOR";
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
	glDepthFunc(GL_ALWAYS);
	
	glEnable(GL_BLEND);								// textúrában az átlátszó rész így nem szolid fekete lesz
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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

	if (memento != nullptr)
	{
		delete memento;
	}
	exit(EXIT_SUCCESS);

	return 0;
}

/*

VAO - Csúcsok eloszlását + indexelést párosítja (VBO + IBO)
TBO - Textúrázást azonosít maximum 16-32 db-ot egymagába. Beállítjuk aktívnak az elsõ elérhetõt és oda fölvesszük a négyzet textúráját.
VBO - Csúcsok eloszlása
IBO - Csúcsok sorrendisége

*/