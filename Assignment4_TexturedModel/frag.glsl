#version 330

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;
// Take in our normal
in vec3 norm;
// We always define a fragment color that we output.
out vec4 fragColor;

uniform int drawMode;
uniform sampler2D tex;

void main() {
	// Default mode
	if (drawMode == 0) {
		fragColor = texture(tex, texCoords);
	} 
	// Wireframe mode
	else if (drawMode == 1) {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	// Texture debug mode
	else if (drawMode == 2) {
		fragColor = vec4(texCoords, 0.0, 1.0);
	}
	else if (drawMode == 3) {
		fragColor = vec4(norm, 1.0);
	}
}
