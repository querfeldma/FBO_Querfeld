#version 430
const float PI = 3.14159265358979323846;

in vec3 Position;
in vec3 Normal;  // World coords.
in vec2 TexCoord;

uniform vec3 CamPos;

layout(binding=0) uniform samplerCube DiffLightTex; //Irradiance Map
layout(binding=1) uniform sampler2D ColorTex; //Cow-Texture

uniform struct MaterialInfo {
    vec3 Color;
} Material;

layout( location = 0 ) out vec4 FragColor;

const vec3 f0 = vec3(0.04);

vec3 schlickFresnel( float dotProd ) {
  return f0 + (1 - f0) * pow(1.0 - dotProd, 5);
}

vec3 schlickFresnelRoughness(float dotProd, float roughness)
{
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1.0 - dotProd, 5.0);
}   

uniform float gamma;
uniform bool use_gamma;
uniform bool use_schlick;
uniform float roughness;
uniform bool use_schlick_r;

void main() {
    vec3 n = normalize(Normal);
    vec3 v = normalize( CamPos - Position );


    vec3 kS = schlickFresnel(max(dot(n, v), 0.0));

    if(use_schlick_r)
        kS = schlickFresnelRoughness(max(dot(n, v), 0.0), roughness);

    vec3 kD = 1.0 - kS;


    //read from lookup-table
    vec3 irradiance = texture(DiffLightTex, n).rgb;

    if(use_gamma)   
        irradiance = pow(irradiance, vec3(1.0f/gamma));

    vec3 diffuse    = irradiance * texture(ColorTex, TexCoord).rgb;
    vec3 ambient    = (kD * diffuse); 


    // Look up incoming radiance from diffuse cube map
    irradiance = texture(DiffLightTex, n).rgb;  //not in sRGB/linear space
    vec3 color = texture(ColorTex, TexCoord).rgb; //sRGB/linear space
        
    if(use_gamma)
        irradiance = pow(irradiance, vec3(1.0f/gamma));

    color *= irradiance;

    if(use_schlick)
        color = ambient;

    FragColor = vec4( color, 1 );
}
