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
in VS_OUT {
	vec3 fragPos;
	vec2 texCoords;
	vec3 norm;
	mat3 tangentToWorld;
} fs_in;

// ~~~~~~~~~~ OUTPUTS ~~~~~~~~~~
out vec4 fragColor;

// ~~~~~~~~~~ UNIFORMS ~~~~~~~~~~
uniform vec3 viewPosition;
uniform int drawMode;
uniform bool hasNormalMap;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform PointLight pointLights[NUM_POINT_LIGHTS];

vec3 allPointLights(vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir); 

void main() {
	// Calculate normal in world space based on normal map
	vec3 normal;
	if (hasNormalMap) {
		normal = texture(normalMap, fs_in.texCoords).rgb;
		normal = normal * 2.0 - 1.0;
		normal = normalize(fs_in.tangentToWorld * normal);
	} else {
		normal = fs_in.norm;
	}

	// Get view direction
	vec3 viewDir = normalize(viewPosition - fs_in.fragPos);

	// Store final texture color
	vec3 diffuseColor = texture(diffuseMap, fs_in.texCoords).rgb;

	// ~~~~~~~~~~ DRAWING MODES ~~~~~~~~~~
	// Default mode
	if (drawMode == 0) {
		vec3 lighting = allPointLights(normal, viewDir);
		fragColor = vec4(diffuseColor * lighting, 1.0);
	} 
	// Wireframe mode
	else if (drawMode == 1) {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	// Texture debug mode
	else if (drawMode == 2) {
		fragColor = vec4(fs_in.texCoords, 0.0, 1.0);
	}
	// Normal debug mode
	else if (drawMode == 3) {
		fragColor = vec4(normal * 0.5 + 0.5, 1.0);
	}
	// Lighting debug mode
	else if (drawMode == 4) {
		vec3 lighting = allPointLights(normal, viewDir);
		fragColor = vec4(lighting, 1.0);
	}
}

vec3 allPointLights(vec3 normal, vec3 viewDir) {
	vec3 lighting = vec3(0);

	for (int ii = 0; ii < NUM_POINT_LIGHTS; ++ii) {
		lighting += calcPointLight(pointLights[ii], normal, viewDir);
	}

	return lighting;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	// (1) Compute ambient light
	vec3 ambient = light.ambientIntensity * light.color;

	// (2) Compute diffuse light
	// From our lights position and the fragment, we can get
	// a vector indicating direction
	// Note it is always good to 'normalize' values.
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	// Now we can compute the diffuse light impact
	float diffImpact = (max(dot(normal, lightDir), 0.0));
	vec3 diffuse = diffImpact * light.color;

	// (3) Compute specular light
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 100);
	vec3 specular = light.specularIntensity * spec * light.color;

	// Calculate attenuation
	float lightDistance = length(light.position - fs_in.fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// Final light intensity
	return (ambient + diffuse + specular);
} 

