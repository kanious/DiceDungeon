#version 460

//uniform mat4 MAT;
uniform mat4 g_matWorld, g_matView, g_matProj;

in layout(location = 0) vec4 vColour;
in layout(location = 1) vec4 vPos;
in layout(location = 2) vec4 vNormal;
in layout(location = 3) vec4 vTexUV;
in layout(location = 4) vec4 vTangent;
in layout(location = 5) vec4 vBinormal;
in layout(location = 6) vec4 vBoneID;
in layout(location = 7) vec4 vBoneWeight;


out vec4 color;
out vec2 texCoord;

void main()
{
	gl_Position = g_matProj * g_matView * g_matWorld * vPos;//vec4(vPos, 1.0);
	//gl_Position = MAT * vec4(vPos, 1.0);
	color = vColour;
	texCoord = vTexUV.xy;
}