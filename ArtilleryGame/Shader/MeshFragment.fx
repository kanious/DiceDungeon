#version 460

in vec3 color;
in vec2 texCoord;

out vec4 daColor;
//out vec4 normal;
//out vec4 specular;

uniform sampler2D diffTexture;

void main()
{
	//gl_FragColor = vec4(color, 1.0);
	//daColor = vec4(color, 1.0);
	daColor = texture(diffTexture, texCoord);
}