#version 330

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;
// And our normal
in vec3 norm;
// And our fragment position for lighting
in vec3 fragPos;

// We always define a fragment color that we output.
out vec4 fragColor;

// Define our light(s)
struct PointLight {
    vec3 color;
    vec3 position;
    float ambientIntensity;
    float specularIntensity;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Maintain our uniforms.
uniform sampler2D tex;              // our primary texture
uniform mat4 view;                  // we need the view matrix for highlights
uniform PointLight pointLights[3];  // Our lights

void main() {
	vec3 lighting = vec3(0);

	// Calculation to get camera position taken from this post: https://community.khronos.org/t/extracting-camera-position-from-a-modelview-matrix/68031
	mat4 inverseView = inverse(view);
	vec3 viewPos = vec3(inverseView[3] / inverseView[3][3]);
	vec3 viewDir = normalize(viewPos - fragPos);
	
	for (int ii = 0; ii < pointLights.length(); ii++) {
		PointLight light = pointLights[ii];
		
		// (1) Compute ambient light
		vec3 ambient = light.ambientIntensity * light.color;

		// (2) Compute diffuse light
		// From our lights position and the fragment, we can get
		// a vector indicating direction
		// Note it is always good to 'normalize' values.
		vec3 lightDir = normalize(light.position - fragPos);
		// Now we can compute the diffuse light impact
		float diffImpact = (max(dot(norm, lightDir), 0.0));
		vec3 diffuse = diffImpact * light.color;

		// (3) Compute specular light
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = light.specularIntensity * spec * light.color;

		// Calculate attenuation
		float lightDistance = length(lightDir);
		float attenuation = 1.0 / (light.constant +
															 light.linear * lightDistance +
															 light.quadratic * (lightDistance * lightDistance));
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		// Final light intensity
		lighting += ambient + diffuse + specular;
	}
	
	// Store final texture color
	vec3 diffuseColor = texture(tex, texCoords).rgb;
	
	// Final color
	if (gl_FrontFacing)
	{
		fragColor = vec4(diffuseColor * lighting, 1.0);	
	}
	else
	{
		fragColor = vec4(diffuseColor * lighting, 1.0);	
	}
}
