#version 430
out vec4 FragColor;

in vec3 localPos; // world coords

layout(binding=0) uniform samplerCube cubeMap;


void main()
{		
    vec4 color = texture(cubeMap, localPos);

    FragColor = color;
}
