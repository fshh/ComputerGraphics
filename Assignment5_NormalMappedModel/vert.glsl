#version 330

// ~~~~~~~~~~ INPUTS ~~~~~~~~~~
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

// ~~~~~~~~~~ OUTPUTS ~~~~~~~~~~
out vec3 fragPos;
out vec2 texCoords;
out vec3 norm;
out mat3 tangentToWorld;

// ~~~~~~~~~~ UNIFORMS ~~~~~~~~~~
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

void main()
{
	// We have our transformed position set properly now
	gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);

	// Fragment position for lighting
	fragPos = (modelMatrix * vec4(position, 1.0)).xyz;

	// And we map our texture coordinates as appropriate
	texCoords = textureCoords;

	// Pass normal to frag
	norm = normalize(mat3(normalMatrix) * normal);

	// Create world-to-tangent space matrix
	vec3 T = normalize(vec3(normalMatrix * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(normalMatrix * vec4(normal, 0.0)));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(N, T);
	tangentToWorld = mat3(T, B, N);
}
