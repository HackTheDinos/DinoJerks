#version 150

uniform vec4 uPickedCol;

in vec2 TexCoord;

out vec4 oColor;

void main(void)
{
	vec4 grayUD = mix( vec4(0,0,0,1), vec4(1,1,1,1), TexCoord.t );
	vec4 finalCol = mix( grayUD, uPickedCol, TexCoord.s);
	oColor = min( grayUD, finalCol );
}