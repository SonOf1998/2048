#pragma once

#pragma once
class GPUProgram
{
private:
	unsigned int shaderProgramId;

	void getErrorInfo(unsigned int) const;
	void checkShader(unsigned int, const char* const) const;
	void checkLinking(unsigned int) const;

public:
	GPUProgram() = default;
	~GPUProgram();

	unsigned int getId() const;
	void create(const char* const, const char* const);
};

