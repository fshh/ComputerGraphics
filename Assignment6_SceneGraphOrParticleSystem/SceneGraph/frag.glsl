#version 330

// ~~~~~~~~~~ CONSTANTS ~~~~~~~~~~
#define NUM_POINT_LIGHTS 1

// ~~~~~~~~~~ STRUCTS ~~~~~~~~~~
struct PointLight {
    vec3 position;
    vec3 color;
    float ambientIntensity;
    float specularIntensity;
    float constant;
    float linear;
    float quadratic;
};

// ~~~~~~~~~~ INPUTS ~~~~~~~~~~
in vec3 fragPos;
in vec2 texCoords;
in vec3 norm;
in mat3 tangentToWorld;

// ~~~~~~~~~~ OUTPUTS ~~~~~~~~~~
out vec4 fragColor;

// ~~~~~~~~~~ UNIFORMS ~~~~~~~~~~
uniform mat4 viewMatrix;
uniform int drawMode;
uniform bool hasNormalMap;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform PointLight pointLights[NUM_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir); 

void main() {
	// Calculate normal in world space based on normal map
	vec3 N;
	if (hasNormalMap) {
		N = texture(normalMap, texCoords).rgb;
		N = N * 2.0 - 1.0;
	} else {
		N = norm;
	}
	N = normalize(tangentToWorld * N);

	// Default mode
	if (drawMode == 0) {
		vec3 lighting = vec3(0);

		// Calculation to get camera position taken from this post: https://community.khronos.org/t/extracting-camera-position-from-a-modelview-matrix/68031
		mat4 inverseView = inverse(viewMatrix);
		vec3 viewPos = vec3(inverseView[3] / inverseView[3][3]);
		vec3 viewDir = normalize(viewPos - fragPos);

		for (int ii = 0; ii < NUM_POINT_LIGHTS; ++ii) {
			lighting += CalcPointLight(pointLights[ii], N, viewDir);
		}

		// Store final texture color
		vec3 diffuseColor = texture(diffuseMap, texCoords).rgb;

		fragColor = vec4(diffuseColor * lighting, 1.0);
		//fragColor = vec4(diffuseColor, 1.0);
		//fragColor = vec4(tangentToWorld[1] * 0.5 + 0.5, 1.0);
		//fragColor = vec4(normalize(pointLights[0].position - fragPos), 1.0);
	} 
	// Wireframe mode
	else if (drawMode == 1) {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	// Texture debug mode
	else if (drawMode == 2) {
		fragColor = vec4(texCoords, 0.0, 1.0);
	}
	// Normal debug mode
	else if (drawMode == 3) {
		fragColor = vec4(N * 0.5 + 0.5, 1.0);
		//fragColor = vec4(norm * 0.5 + 0.5, 1.0);
	}
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	// (1) Compute ambient light
	vec3 ambient = light.ambientIntensity * light.color;

	// (2) Compute diffuse light
	// From our lights position and the fragment, we can get
	// a vector indicating direction
	// Note it is always good to 'normalize' values.
	vec3 lightDir = normalize(light.position - fragPos);
	// Now we can compute the diffuse light impact
	float diffImpact = (max(dot(normal, lightDir), 0.0));
	vec3 diffuse = diffImpact * light.color;

	// (3) Compute specular light
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = light.specularIntensity * spec * light.color;

	// Calculate attenuation
	float lightDistance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// Final light intensity
	return (ambient + diffuse + specular);
} 

