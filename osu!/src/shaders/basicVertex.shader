#version 460 core

layout(location = 0) in vec3 trianglePos;
layout(location = 1) in vec4 colorCoords;
layout(location = 2) in vec2 texCoords;

uniform mat4 scaleMat4;

out vec4 color;
out vec2 textureCoords;

void main(){

	gl_Position = /*scaleMat4 * */vec4(trianglePos, 1.0f);
	color = colorCoords;
	textureCoords = texCoords;
}