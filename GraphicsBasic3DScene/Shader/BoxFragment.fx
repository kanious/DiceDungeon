#version 460

in vec4 fColour;

uniform sampler2D diffTexture;

out vec4 daColor;
void main()
{
	//vec4 diffColour = texture(diffTexture, fTexUV.xy);

	//daColor = fColour;
	//daColor = texture(diffTexture, fColour.xy);
	daColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}