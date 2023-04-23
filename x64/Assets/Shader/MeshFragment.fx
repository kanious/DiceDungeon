#version 460

in vec4 fColour;
in vec4 fNormal;
in vec4 fVtxWorldPos;
in vec4 fTexUV;
in vec4 fTangent;
in vec4 fBinormal;
in mat4 fInvMat;

uniform sampler2D diffTexture;
uniform sampler2D normalTexture;

uniform bool isLightEnable;
uniform bool isSelected;
uniform bool isTransparency;
uniform bool isNormal;

out vec4 FragColour[3];
void main()
{
	FragColour[0] = texture(diffTexture, fTexUV.xy);
	FragColour[1] = fNormal;
	FragColour[2] = fVtxWorldPos;
}
