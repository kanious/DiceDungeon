#version 460

in vec3 color;

out vec4 daColor;
//out vec4 normal;
//out vec4 specular;

void main()
{
	//gl_FragColor = vec4(color, 1.0);
	daColor = vec4(color, 1.0);
}