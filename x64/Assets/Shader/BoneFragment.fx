#version 460

in vec4 fColour;
in vec4 fNormal;
in vec4 fVtxWorldPos;
in vec4 fTexUV;
in vec4 fTangent;
in vec4 fBinormal;
in mat4 fInvMat;
in float debugColor;

uniform sampler2D diffTexture;
uniform sampler2D normalTexture;

uniform bool isSelected;

out vec4 daColor;
void main()
{
	if (!isSelected)
	{
		vec4 diffColour = texture(diffTexture, fTexUV.xy);
		daColor = vec4(diffColour.xyz, 1.0f);
	}
	else
	{
		if (int(debugColor / 5) == 0) daColor = vec4(1, 0, 0, 1);
		if (int(debugColor / 5) == 1) daColor = vec4(0, 1, 0, 1);
		if (int(debugColor / 5) == 2) daColor = vec4(0, 0, 1, 1);
		if (int(debugColor / 5) == 3) daColor = vec4(1, 1, 0, 1);
		if (int(debugColor / 5) == 4) daColor = vec4(0, 1, 1, 1);
		if (int(debugColor / 5) == 5) daColor = vec4(1, 0, 1, 1);

		if (int(debugColor / 5) == 6) daColor = vec4(0.5, 0, 0, 1);
		if (int(debugColor / 5) == 7) daColor = vec4(0, 0.5, 0, 1);
		if (int(debugColor / 5) == 8) daColor = vec4(0, 0, 0.5, 1);
		if (int(debugColor / 5) == 9) daColor = vec4(0.5, 0.5, 0, 1);
		if (int(debugColor / 5) == 10) daColor = vec4(0, 0.5, 0.5, 1);
		if (int(debugColor / 5) == 11) daColor = vec4(0.5, 0, 0.5, 1);

		if (int(debugColor / 5) == 12) daColor = vec4(1, 1, 1, 1);
		if (int(debugColor / 5) == 13) daColor = vec4(0.5, 0.5, 0.5, 1);
		if (int(debugColor / 5) >= 14) daColor = vec4(0, 0, 0, 1);
	}
}