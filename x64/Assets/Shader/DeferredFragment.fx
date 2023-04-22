#version 460

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform vec4 cameraPosition;

struct sLight
{
	vec4       position;
	vec4       direction;
	vec4       diffuse;
	vec4       specular;		// rgp = highlight color, w = power
	vec4       ambient;			// rgp = ambient color, w = power
	vec4       atten;			// x = constant, y = linear, z = quadratic, w = cutoff range
	vec4       param1;			// x = lightType, y = inner angle, z = outer angle, w = TBD(?)
	vec4       param2;			// x = turn (1:on, 0:off)
};
const int DIRECTIONAL_LIGHT_TYPE = 0;
const int POINT_LIGHT_TYPE = 1;
const int SPOT_LIGHT_TYPE = 2;
const int NUMBEROFLIGHTS = 20;
uniform sLight theLights[NUMBEROFLIGHTS];

vec4 calculateLight(vec3 diffuseColor, vec3 normal, vec3 vtxWorldPos);
void main()
{
	vec4 image = texture(diffuseTexture, TexCoords);
	vec4 normal = texture(normalTexture, TexCoords);
	vec4 position = texture(positionTexture, TexCoords);
	//FragColor = image;
	//return;

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

	vec4 lightDiffuse = vec4(1.f, 1.f, 1.f, 1.f);
	vec3 lightContrib = lightDiffuse.rgb;
	vec3 directionDir = vec3(0.5f, -1.f, 0.5f);
	vec3 norm = normal.xyz;
	norm = normalize(norm);
	directionDir = normalize(directionDir);

	//vec3 light = vec3(50, 100, 50);
	//vec3 lightDir = light - position.xyz;
	//directionDir = normalize(lightDir);

	float dotValue = dot(-directionDir, norm);
	dotValue = max(0.f, dotValue);
	float ambientPower = lightDiffuse.w * 0.2f;
	lightContrib *= (dotValue + ambientPower);

	float lightPower = lightDiffuse.w;

	vec3 result = image.xyz * lightDiffuse.xyz * lightContrib * lightPower;
	FragColor.xyz = result;
	FragColor.a = image.a;

	//-------------------------------------- old light 2

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