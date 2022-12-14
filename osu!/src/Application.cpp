#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib")

#include <iostream>

#include "BeatMap.h"
//#include "Shader.h"
//#include "osuCircleStatic.h"
//#include "Assert.h"

//#include <glm/glm.hpp>
//#include "glm/gtx/transform.hpp"
//#include "glm/gtc/type_ptr.hpp"

#ifdef DEBUG

#define GLFW_DEBUG_WINDOW_HINT glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true)
#define GL_DEBUG_EXT int debugStatus;\
	glGetIntegerv(GLFW_OPENGL_DEBUG_CONTEXT, &debugStatus);\
	if (debugStatus & GL_CONTEXT_FLAG_DEBUG_BIT)\
	{\
		glEnable(GL_DEBUG_OUTPUT);\
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);\
		glDebugMessageCallback(glDebugOutput, nullptr);\
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);\
	}
#define GL_DEBUG_CALLBACK void APIENTRY glDebugOutput(GLenum source,\
	GLenum type,\
	unsigned int id,\
	GLenum severity,\
	GLsizei length,\
	const char* message,\
	const void* userParam)\
	{\
	/* ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;*/\
	std::cout << "---------------" << std::endl;\
	std::cout << "Debug message (" << id << "): " << message << std::endl;\
	\
	switch (source)\
	{\
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;\
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;\
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;\
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;\
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;\
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;\
	} std::cout << std::endl;\
\
	switch (type)\
	{\
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;\
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;\
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;\
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;\
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;\
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;\
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;\
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;\
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;\
	} std::cout << std::endl;\
\
	switch (severity)\
	{\
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;\
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;\
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;\
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;\
	} std::cout << std::endl;\
	std::cout << std::endl;\
	}

#else

#define GLFW_DEBUG_WINDOW_HINT
#define GL_DEBUG_EXT
#define GL_DEBUG_CALLBACK

#endif



GL_DEBUG_CALLBACK

int main()
{
	
	if (glfwInit() == GLFW_FALSE)
		std::cout << "GLFW init failed!" << std::endl;

	glfwSwapInterval(1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFW_DEBUG_WINDOW_HINT;

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "osu!", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	GL_DEBUG_EXT;

	const unsigned char* version = glGetString(GL_VERSION);
	std::cout << version << std::endl;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wire

	

	Game* game = new Game(window, 125, 4);

	glfwSetWindowUserPointer(window, game);

	glfwSetKeyCallback(window, game->KeyboardInput);
	glfwSetMouseButtonCallback(window, game->MouseInput);

	while (!glfwWindowShouldClose(window))
	{

		ASSERT(glClearColor(0.0f, 0.0f, 0.15f, 1.0f));
		ASSERT(glClear(GL_COLOR_BUFFER_BIT));

		game->Draw();

		glfwSwapBuffers(window);

		glfwPollEvents();

	}

	delete game;

	glfwTerminate();

	return 0;
}