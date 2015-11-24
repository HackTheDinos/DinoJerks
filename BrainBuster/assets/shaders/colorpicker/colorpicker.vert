#version 150

uniform mat4 ciModelViewProjection;

in vec4 	ciPosition;
in vec2 	ciTexCoord0;

out highp vec2 TexCoord;

void main(void)
{
	gl_Position = ciModelViewProjection * ciPosition;
	TexCoord = ciTexCoord0;
}