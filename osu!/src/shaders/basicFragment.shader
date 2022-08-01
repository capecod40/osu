#version 460 core

out vec4 triangleColor;

in vec4 color;
in vec2 textureCoords;

uniform sampler2D useTexture;

void main(){

	if (textureCoords.x == -1.0f)
	{
		triangleColor = color;
	}
	else
	{
		triangleColor = texture(useTexture, textureCoords);
	}

}