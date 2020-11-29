#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Position; // world coords
out vec3 Normal;  // In world coords.

uniform mat4 view;
uniform mat4 projection;

void main() {
    Position = VertexPosition;  
    gl_Position = projection * view * vec4(VertexPosition,1.0);
}
