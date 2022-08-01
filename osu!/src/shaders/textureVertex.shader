#version 460 core

layout(location = 0) in vec3 trianglePos;
layout(location = 1) in vec2 textureCoords;

out vec2 texCoords;

uniform mat4 orthoMatrix;

void main() {

	gl_Position = orthoMatrix * vec4(trianglePos, 1.0f);
	texCoords = textureCoords;
}