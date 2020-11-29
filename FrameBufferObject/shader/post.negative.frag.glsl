#version 410

layout (location=0) in vec2 textureCoords;
layout (location=0) out vec4 FragColor;

uniform sampler2D overlayTexture;

void main() {
	vec4 texColor = 1.0f - texture(overlayTexture, textureCoords);

	FragColor = vec4(texColor.xyz, 1.0f);
}
