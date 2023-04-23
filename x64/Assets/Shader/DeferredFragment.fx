#version 460

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

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
const int NUMBEROFLIGHTS = 1;
uniform sLight theLights[NUMBEROFLIGHTS];
uniform vec4 eyeLocation;

vec4 calculateLight(vec3 diffuseColor, vec3 normal, vec3 vtxWorldPos);
void main()
{
	vec4 image = texture(diffuseTexture, TexCoords);
	vec4 normal = texture(normalTexture, TexCoords);
	vec4 position = texture(positionTexture, TexCoords);

	vec3 vDiffuse = image.xyz;
	vec3 vNormal = normal.xyz;
	vec3 vPos = position.xyz;

	FragColor = calculateLight(vDiffuse, vNormal, vPos);
}

vec4 calculateLight(vec3 diffuseColor, vec3 normal, vec3 vtxWorldPos)
{
	vec4 finalColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	vec3 norm = normalize(normal);
	for (int i = 0; i < NUMBEROFLIGHTS; ++i)
	{
		if (0.0f == theLights[i].param2.x)
			continue;

		int lightType = int(theLights[i].param1.x);

		//DIRECTIONAL_LIGHT_TYPE
		if (lightType == DIRECTIONAL_LIGHT_TYPE)
		{
			vec3 lightContrib = theLights[i].diffuse.rgb;
			vec3 directionalDir = theLights[i].direction.xyz;
			directionalDir = normalize(directionalDir);
			float dot = dot(-directionalDir, norm);
			dot = max(0.0f, dot);
			float ambientPower = theLights[i].diffuse.w * 0.1f;
			lightContrib *= (dot + ambientPower);

			float lightPower = theLights[i].diffuse.w;
			vec3 result = diffuseColor * theLights[i].diffuse.xyz * lightContrib * lightPower;
			finalColour.xyz += result;

			continue;
		}

		//POINT_LIGHT_TYPE
		vec3 vLightToVertex = theLights[i].position.xyz - vtxWorldPos;
		float distanceToLight = length(vLightToVertex);
		//if (theLights[i].atten.w < distanceToLight)
		//	continue;

		vec3 lightDir = normalize(vLightToVertex);
		float dotPoint = dot(lightDir, normal);
		dotPoint = max(0.0f, dotPoint);
		float lightPower = theLights[i].diffuse.w;
		vec3 lightDiffuseContrib = dotPoint * theLights[i].diffuse.rgb * lightPower;

		//specular
		vec3 lightSpecularContrib = vec3(0.0f);
		vec3 vReflect = reflect(-lightDir, norm);
		vec3 vEye = normalize(eyeLocation.xyz - vtxWorldPos);
		float specularPower = theLights[i].specular.w;
		lightSpecularContrib =
			pow(max(0.0f, dot(vEye, vReflect)), specularPower) *
			theLights[i].specular.rgb;

		float attenuation = 1.0f /
			(theLights[i].atten.x +
				theLights[i].atten.y * distanceToLight +
				theLights[i].atten.z * distanceToLight * distanceToLight);

		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;

		if (lightType == SPOT_LIGHT_TYPE)
		{
			vec3 vtxToLight = vtxWorldPos - theLights[i].position.xyz;
			vtxToLight = normalize(vtxToLight);
			float currentRayAngle =
				dot(vtxToLight, theLights[i].direction.xyz);
			currentRayAngle = max(0.0f, currentRayAngle);

			float phi = cos(radians(theLights[i].param1.z));
			float theta = cos(radians(theLights[i].param1.y));

			// outside of outerCone
			if (currentRayAngle < phi)
			{
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			// between innerCone and outerCone
			else if (currentRayAngle < theta)
			{
				//float penumbraRatio = (currentRayAngle - phi) /
				//	(theta - phi);

				float epsilon = theta - phi;
				float penumbraRatio = clamp((currentRayAngle - phi) / epsilon, 0.0, 1.0);

				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}

		}

		vec3 result = (diffuseColor * lightDiffuseContrib) + (theLights[i].specular.xyz * lightSpecularContrib);
		finalColour.xyz += result;
	}

	//finalColour.r = min(1.0f, finalColour.r);
	//finalColour.g = min(1.0f, finalColour.g);
	//finalColour.b = min(1.0f, finalColour.b);
	finalColour.a = 1.0f;

	return finalColour;
}