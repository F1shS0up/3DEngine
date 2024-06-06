#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec2 TexCoords;
out vec3 FragPos;  
out mat3 TBN;
out vec3 Normal;  
out vec4 FragPosLightSpace;

in DATA
{
    vec2 TexCoords;
    vec3 FragPos;  
    vec3 Normal;  
    vec4 FragPosLightSpace;

    mat4 model;

} data_in[];


void main()
{
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].TexCoords - data_in[0].TexCoords;
    vec2 deltaUV1 = data_in[2].TexCoords - data_in[0].TexCoords;

    // one over the determinant
    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(Normal);

    mat3 _TBN = mat3(T, B, N);
    TBN = transpose(_TBN);

    gl_Position = gl_in[0].gl_Position;
    TexCoords = data_in[0].TexCoords;
    FragPos = data_in[0].FragPos;
    Normal = data_in[0].Normal;
    FragPosLightSpace = data_in[0].FragPosLightSpace;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    TexCoords = data_in[1].TexCoords;
    FragPos = data_in[1].FragPos;
    Normal = data_in[1].Normal;
    FragPosLightSpace = data_in[1].FragPosLightSpace;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    TexCoords = data_in[2].TexCoords;
    FragPos = data_in[2].FragPos;
    Normal = data_in[2].Normal;
    FragPosLightSpace = data_in[2].FragPosLightSpace;
    EmitVertex();

    EndPrimitive();
}