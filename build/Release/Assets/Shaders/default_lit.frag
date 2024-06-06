#version 460 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    sampler2D diffuseMap;
    vec3 specular;   
    sampler2D specularMap;
    float shininess;
}; 

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float farPlane;

    bool castShadows;
};
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool castShadows;
};  

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;  
in vec4 FragPosLightSpace;
  
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[8];
uniform int pointLightCount;

layout(binding = 2)uniform sampler2D shadowMap;
layout(binding = 3)uniform samplerCubeArray cubeArray;

float DirectionalShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 normal = normalize(Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int sampleRadius = 1;
    for(int x = -sampleRadius; x <= sampleRadius; x++)
    {
        for(int y = -sampleRadius; y <= sampleRadius; y++)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= pow((sampleRadius * 2 + 1), 2);
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);
float PointShadowCalculation(vec4 fragPosLightSpace, PointLight light, vec3 lightDir, int index)
{
       // get vector between fragment position and light position
    vec3 fragToLight = FragPos - light.position;
  
    float currentDepth = length(fragToLight);
   
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / light.farPlane)) / 25.0;
    for(int i = 0; i < samples; i++)
    {
        float closestDepth = texture(cubeArray, vec4(vec3(fragToLight + gridSamplingDisk[i] * diskRadius), index)).r;
        closestDepth *= light.farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
   
    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
    vec3 specular = light.specular * spec * texture(material.specularMap, TexCoords).rgb * material.specular;

    float shadow = DirectionalShadowCalculation(FragPosLightSpace, lightDir);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));     
    
    return lighting;
}  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
    vec3 diffuse  = light.diffuse * diff * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
    vec3 specular = light.specular * spec * texture(material.specularMap, TexCoords).rgb * material.specular;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    float shadow = light.castShadows ? PointShadowCalculation(FragPosLightSpace, light, lightDir, index) : 0;                                
    vec3 lighting = vec3(ambient + (1.0 - shadow) * (diffuse + specular));     

    return lighting;
} 

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < pointLightCount; i++){
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, i);
    }
    
    FragColor = vec4(result, 1.0);
} 