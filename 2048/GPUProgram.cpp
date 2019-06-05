#include "pch.h"
#include "GPUProgram.h"

#include <glew.h>
#include <iostream>


void GPUProgram::getErrorInfo(unsigned int handle) const
{
	int logLen, written;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		char * log = new char[logLen];
		glGetShaderInfoLog(handle, logLen, &written, log);
		printf("Shader log:\n%s", log);
		delete log;
	}
}

void GPUProgram::checkShader(unsigned int shader, const char* const message) const
{
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);

	if (!OK)
	{
		printf("%s!\n", message);
		getErrorInfo(shader);
		getchar();
	}
}

void GPUProgram::checkLinking(unsigned int program) const
{
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);

	if (!OK)
	{
		printf("Failed to link shader program!\n");
		getErrorInfo(program);
		getchar();
	}
}

GPUProgram::GPUProgram()
{
	shaderProgramId = 0;
}


GPUProgram::~GPUProgram()
{
	glDeleteProgram(shaderProgramId);
}

unsigned int GPUProgram::getId() const
{
	return shaderProgramId;
}

void GPUProgram::create(const char* const vertexSource, const char* const fragmentSource, const char* const fragmentShaderOutputName)
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (!vertexShader) {
		printf("Error in vertex shader creation\n");
		exit(1);
	}
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	checkShader(vertexShader, "Vertex shader error");

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragmentShader) {
		printf("Error in fragment shader creation\n");
		exit(1);
	}

	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	checkShader(fragmentShader, "Fragment shader error");

	shaderProgramId = glCreateProgram();
	if (!shaderProgramId) {
		printf("Error in shader program creation\n");
		exit(1);
	}
	glAttachShader(shaderProgramId, vertexShader);
	glAttachShader(shaderProgramId, fragmentShader);

	glBindFragDataLocation(shaderProgramId, 0, fragmentShaderOutputName);	

	glLinkProgram(shaderProgramId);
	checkLinking(shaderProgramId);

	glUseProgram(shaderProgramId);
}

void GPUProgram::use() const
{
	glUseProgram(shaderProgramId);
}
