#version 410

layout (location=0) in vec2 textureCoords;
layout (location=0) out vec4 FragColor;

uniform sampler2D overlayTexture;

void main() {
	vec4 texColor = texture(overlayTexture, textureCoords);

	FragColor = vec4(texColor.zyx, 1.0f);
}
