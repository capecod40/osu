#version 460 core

layout(location = 0) in vec3 trianglePos;
layout(location = 1) in vec4 colorCoords;

out vec4 color;

uniform mat4 orthoMatrix;
uniform mat4 shrinkMatrix;
uniform mat4 toOrigin;
uniform mat4 fromOrigin;

void main(){

	gl_Position = orthoMatrix * fromOrigin * shrinkMatrix * toOrigin * vec4(trianglePos, 1.0f);
	color = colorCoords;
}