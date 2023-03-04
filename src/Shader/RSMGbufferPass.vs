#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent_tS;
layout (location = 4) in vec3 biTangent_tS;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;




out VS_OUT {
     vec2 TexCoords;
     vec3 WorldPos;
     vec3 Normal;
} vs_out;




void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(model) * aNormal;   

    gl_Position =  lightSpaceMatrix * vec4(vs_out.WorldPos, 1.0);
}