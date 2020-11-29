#version 430 core
out vec4 FragColor;

in vec3 localPos;
  
layout(binding=1) uniform samplerCube environmentMap;
  
void main()
{
    vec3 envColor = texture(environmentMap, localPos).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));  //gamma correction
  
    FragColor = vec4(envColor, 1.0);
    FragColor = texture(environmentMap, localPos);
}