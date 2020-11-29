#version 410

layout (location=0) in vec2 VertexPosition;
layout (location=0) out vec2 textureCoords;

uniform mat4 transformationMatrix;

void main()
{
	gl_Position = transformationMatrix * vec4(VertexPosition, 0.0, 1.0);
	textureCoords = vec2((VertexPosition.x+1.0)/2.0, (VertexPosition.y+1.0)/2.0);
}
