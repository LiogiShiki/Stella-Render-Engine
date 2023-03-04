#version 330 core

in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D finalColor;

void main()
{
    vec3 color = texture2D(finalColor,TexCoords).rgb;
    FragColor = vec4(color,1.0);
}