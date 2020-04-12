#version 330

// ~~~~~~~~~~ INPUTS ~~~~~~~~~~
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

// ~~~~~~~~~~ OUTPUTS ~~~~~~~~~~
out vec2 texCoords;
out vec3 norm;

// ~~~~~~~~~~ UNIFORMS ~~~~~~~~~~
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	// We have our transformed position set properly now
	gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);
	// And we map our texture coordinates as appropriate
	texCoords = textureCoords;
	// Pass normal to frag
	norm = normal;
}
