#version 330

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;
// Take in our normal
in vec3 norm;
// We always define a fragment color that we output.
out vec4 fragColor;

uniform int drawMode;
uniform bool hasNormalMap;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main() {
	// Default mode
	if (drawMode == 0) {
		fragColor = texture(diffuseMap, texCoords);
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
		if (hasNormalMap) {
			fragColor = texture(normalMap, texCoords);
		} else {
			fragColor = vec4(norm * 0.5 + 0.5, 1.0);
		}
	}
}
