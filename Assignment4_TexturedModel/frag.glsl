#version 330

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;
// We always define a fragment color that we output.
out vec4 fragColor;

uniform bool wireframe;
uniform sampler2D tex;

void main() {
  // Set our output fragment color to whatever we pull from our input texture (Note, change 'tex' to whatever the sampler is named)
	if (wireframe) {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	} else {
		fragColor = texture(tex, texCoords);
	}
}
