#version 150

uniform mat4 ciModelViewProjection;
in vec4 	 ciPosition;

out vec4 vPos;

void main(void)
{
	vPos = ciPosition;
	gl_Position = ciModelViewProjection * ciPosition;
}