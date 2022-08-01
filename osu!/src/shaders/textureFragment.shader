#version 460 core

out vec4 triangleColor;

in vec2 texCoords;

uniform sampler2D textureSampler;

void main(){

	triangleColor = texture(textureSampler, texCoords);

}