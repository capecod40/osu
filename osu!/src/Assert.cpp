#include "Assert.h"

#include "glad/glad.h"
#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLPrintError()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "OpenGL Error : " << error << std::endl;
		return false;
	}
	else
		return true;
}