#version 460

//uniform mat4 MAT;
uniform mat4 g_matWorld, g_matView, g_matProj;

in layout(location = 0) vec3 vPos;
in layout(location = 1) vec3 vCol;
out vec3 color;

void main()
{
	gl_Position = g_matProj * g_matView * g_matWorld * vec4(vPos, 1.0);
	//gl_Position = MAT * vec4(vPos, 1.0);
	color = vCol;
}