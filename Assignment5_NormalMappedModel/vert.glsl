#version 330

// ~~~~~~~~~~ INPUTS ~~~~~~~~~~
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

// ~~~~~~~~~~ OUTPUTS ~~~~~~~~~~
out VS_OUT {
	vec3 fragPos;
	vec2 texCoords;
	vec3 norm;
	vec3 tang;
	mat3 tangentToWorld;
} vs_out;

// ~~~~~~~~~~ UNIFORMS ~~~~~~~~~~
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

void main()
{
	// Output fragment position
	vs_out.fragPos = vec3(modelMatrix * vec4(position, 1.0));

	// Output texture coords
	vs_out.texCoords = textureCoords;

	// Output normal (in world space)
	vs_out.norm = normalize(normalMatrix * normal);

	// Create world-to-tangent space matrix
	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * normal);
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	vs_out.tang = T;
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(N, T);
	vs_out.tangentToWorld = mat3(T, B, N);

	// Output vertex position
	gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);
}
