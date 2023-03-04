#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

void main()
{
    gl_Position = model * vec4(aPos, 1.0);
}