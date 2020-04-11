#version 330

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;

// We always define a fragment color that we output.
out vec4 fragColor;

// Maintain our uniforms.
uniform sampler2D FBOTex;              // our primary texture

const float offset = 1.0 / 300.0;

void main() {
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),	// top left
		vec2(0.0f, offset),		// top middle
		vec2(offset, offset),	// top right
		vec2(-offset, 0.0f),	// middle left
		vec2(0.0f, 0.0f),		// middle middle
		vec2(offset, 0.0f),		// middle right
		vec2(-offset, -offset),	// bottom left
		vec2(0.0f, -offset),	// bottom middle
		vec2(offset, -offset)	// bottom right
	);

	float kernel[9] = float[](
		2,   2, 2,
		2, -15, 2,
		2,   2, 2
	);

	vec3 sampleTex[9];
	for (int ii = 0; ii < 9; ii++) {
		sampleTex[ii] = vec3(texture(FBOTex, texCoords.st + offsets[ii]));
	}

	vec3 col = vec3(0.0);
	for (int ii = 0; ii < 9; ii++) {
		col += sampleTex[ii] * kernel[ii];
	}

	fragColor = vec4(col, 1.0);

	//fragColor = vec4(texture(FBOTex, texCoords).rgb, 1.0);
}