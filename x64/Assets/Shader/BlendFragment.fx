#version 460

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D blendTexture;

void main()
{
	//vec4 image = texture(blendTexture, TexCoords);
	//FragColor = vec3(image.x, image.y + 1.f, image.z, image.w);
	FragColor = vec3(0.f, 0.f, 1.f, 1.f);
	return;

	//vec3 FragPos = texture(positionTexture, TexCoords).rgb;
	//vec3 Normal = texture(normalTexture, TexCoords).rgb;
	//vec3 Diffuse = texture(diffuseTexture, TexCoords).rgb;
	//float Specular = texture(diffuseTexture, TexCoords).a;

	//vec3 lighting = Diffuse * 0.1f;
	//vec3 viewDir = normalize(cameraPosition.xyz - FragPos);

	//vec3 lightPos = vec3(50, 100, 50);
	//vec3 lightDir = normalize(lightPos - FragPos);
	//vec3 lightDiffuse = vec3(1.f, 1.f, 1.f);

	//vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightDiffuse;
	//vec3 halfwayDir = normalize(lightDir + viewDir);
	//float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
	//vec3 specular = lightDiffuse * spec * Specular;

	//lighting += diffuse + specular;
	//FragColor = vec4(lighting, 1.f);

	//--------------------------------------

	//vec4 lightDiffuse = vec4(1.f, 1.f, 1.f, 1.f);
	//vec3 lightContrib = lightDiffuse.rgb;
	//vec3 directionDir = vec3(0.5f, -1.f, 0.5f);
	//vec3 norm = normal.xyz;
	//norm = normalize(norm);
	//directionDir = normalize(directionDir);

	//vec3 light = vec3(50, 100, 50);
	//vec3 lightDir = light - position.xyz;
	//directionDir = normalize(lightDir);

	//float dotValue = dot(directionDir, norm.xyz);
	//dotValue = max(0.f, dotValue);
	//lightContrib *= dotValue;

	//float lightPower = lightDiffuse.w;

	//vec3 result = image.xyz * lightDiffuse.xyz * lightContrib * lightPower;
	//FragColor.xyz = result;
	//FragColor.a = image.a;

	//----------------------- old light
	//vec3 light = vec3(50, 100, 50);
	//vec3 lightDir = light - position.xyz;

	//normal = normalize(normal);
	//lightDir = normalize(lightDir);

	//vec3 eyeDir = normalize(cameraPosition.xyz - position.xyz);
	//vec3 vHalfVector = normalize(lightDir.xyz + eyeDir);

	//FragColor = max(dot(normal.xyz, lightDir), 0) * image + pow(max(dot(normal.xyz, vHalfVector), 0.0), 100) * 1.5;


	//---------------------------------------------------------------------------
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//return;

	//vec3 col = texture(diffuseTexture, TexCoords).rgb;
	//FragColor = vec4(col, 1.0);
	//FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}