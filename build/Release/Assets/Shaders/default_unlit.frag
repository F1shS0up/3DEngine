#version 330 core
out vec4 FragColor;

struct Material{
    vec3 ambient;
    sampler2D map;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform Material material;

void main()
{    
    FragColor = vec4(material.ambient * texture(material.map, TexCoords).rgb, 1.0);
}