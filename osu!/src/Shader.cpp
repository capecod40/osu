#include "Shader.h"

#include <glm/glm.hpp>
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Shader::Shader()
{
}

Shader::Shader(std::string vertexFilepath, std::string fragmentFilepath)
	: programID(glCreateProgram()), vertexSource(nullptr), fragmentSource(nullptr)
{

	// vertex shader
	std::ifstream vertexFile;
	vertexFile.open(vertexFilepath);
	if (vertexFile.fail())
	{
		std::cout << "Failed to open vertex shader file!" << std::endl;
		__debugbreak();
	}

	std::stringstream ssVertex;
	ssVertex << vertexFile.rdbuf();
	std::string vertexString = ssVertex.str();
	vertexSource = vertexString.c_str();
	vertexFile.close();
	
	unsigned int vertexID;
	ASSERT(vertexID = glCreateShader(GL_VERTEX_SHADER));
	ASSERT(glShaderSource(vertexID, 1, &vertexSource, 0));
	ASSERT(glCompileShader(vertexID));

	int status;
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		std::cout << "Vertex shader failed to compile!" << std::endl;
		int length;
		glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &length);
		char* infoLog = new char[length];
		glGetShaderInfoLog(vertexID, length, &length, infoLog);
		std::cout << infoLog << std::endl;
		delete infoLog;
		__debugbreak();
	}

	ASSERT(glAttachShader(programID, vertexID));



	// fragment shader
	std::ifstream fragmentFile;
	fragmentFile.open(fragmentFilepath);
	if (fragmentFile.fail())
	{
		std::cout << "Failed to open vertex shader file!" << std::endl;
		__debugbreak();
	}

	std::stringstream ssFragment;
	ssFragment << fragmentFile.rdbuf();
	std::string fragmentString = ssFragment.str();
	fragmentSource = fragmentString.c_str();
	fragmentFile.close();

	unsigned int fragmentID;
	ASSERT(fragmentID = glCreateShader(GL_FRAGMENT_SHADER));
	ASSERT(glShaderSource(fragmentID, 1, &fragmentSource, 0));
	ASSERT(glCompileShader(fragmentID));

	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		std::cout << "Fragment shader failed to compile!" << std::endl;
		int length;
		glGetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &length);
		char* infoLog = new char[length];
		glGetShaderInfoLog(fragmentID, length, &length, infoLog);
		std::cout << infoLog << std::endl;
		delete infoLog;
		__debugbreak();
	}

	ASSERT(glAttachShader(programID, fragmentID));



	ASSERT(glLinkProgram(programID));
	glGetProgramiv(programID, GL_LINK_STATUS, &status);
	if (!status) {
		char infoLog[512];
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::cout << "Program error : " << infoLog << std::endl;
		__debugbreak();
	}
	ASSERT(glDeleteShader(vertexID));
	ASSERT(glDeleteShader(fragmentID));
}

void Shader::useProgram() const
{
	ASSERT(glUseProgram(programID));
}