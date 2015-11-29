#version 150

in vec4 vPos;
out vec4 oColor;

uniform mat4 ciModelViewProjection;

void main(void)
{
	vec4 ssPos = ciModelViewProjection * vPos;

//	vec4 tmpCol = vec4(vPos.x,vPos.y,vPos.z,1);
//	vec4 finalCol = mix( tmpCol, vec4(1,1,1,1), ssPos.z * 0.25 );
	oColor = vec4( vPos.x + 0.25, vPos.y + 0.5, vPos.z * 2.0, 1.0);
}