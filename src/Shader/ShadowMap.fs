#version 460 core

void main()
{             
    gl_FragDepth = gl_FragCoord.z;
    //gl_FragDepth = 0.5f;
}