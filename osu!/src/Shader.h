#pragma once

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Assert.h"

class Shader
{
private:
	unsigned int programID;
	const char* vertexSource;
	const char* fragmentSource;
public:
	Shader();
	Shader(std::string vertexFilepath, std::string fragmentFilepath);

	void useProgram() const;
	inline void unBind() const { glUseProgram(0); }

	inline unsigned int getProgramID() const { return programID; }
};

