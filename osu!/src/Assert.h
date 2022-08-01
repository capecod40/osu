#pragma once

#ifdef DEBUG

#define ASSERT(x) GLClearError();\
	x;\
	if (!GLPrintError()) __debugbreak()

#else

#define ASSERT(x) x

#endif

void GLClearError();
bool GLPrintError();