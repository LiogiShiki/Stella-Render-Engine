#version 450 core

layout(binding=0) uniform sampler2D currentColor;
layout(binding=1) uniform sampler2D previousColor;
layout(binding=2) uniform sampler2D velocityTexture;
layout(binding=3) uniform sampler2D currentDepth;

layout (location = 0) out vec4 History;


uniform float ScreenWidth;
uniform float ScreenHeight;
uniform int frameCount;

in  vec2 screenPosition;
//out vec4 outColor;

vec2 getClosestOffset()
{
    vec2 deltaRes = vec2(1.0 / ScreenWidth, 1.0 / ScreenHeight);
    float closestDepth = 1.0f;
    vec2 closestUV = screenPosition;

    for(int i=-1;i<=1;++i)
    {
        for(int j=-1;j<=1;++j)
        {
            vec2 newUV = screenPosition + deltaRes * vec2(i, j);

            float depth = texture2D(currentDepth, newUV).x;

            if(depth < closestDepth)
            {
                closestDepth = depth;
                closestUV = newUV;
            }
        }
    }

    return closestUV;
}

void main()
{
    vec3 nowColor = texture(currentColor, screenPosition).rgb;
    if(frameCount <= 1)
    {
        History = vec4(nowColor, 1.0);
        return;
    }

    vec2 velocity = texture(velocityTexture, getClosestOffset()).rg;
    vec2 offsetUV = clamp(screenPosition - velocity, 0, 1);
    vec3 preColor = texture(previousColor, offsetUV).rgb;
    float c = 0.05;
    History =vec4(c * nowColor + (1-c) * preColor, 1.0);//vec4(screenPosition,0.0,1.0);//vec4(c * nowColor + (1-c) * preColor, 1.0);//vec4(velocity,0.0,1.0);
}