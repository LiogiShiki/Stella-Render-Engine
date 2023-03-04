#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent_tS;
layout (location = 4) in vec3 biTangent_tS;

layout (std140) uniform Matrices
{
    mat4 u_projection;
    mat4 u_view;
};


layout (std140) uniform Halton_23
{
	vec4 Halton[16];
};

out VS_OUT {
     vec2 TexCoords;
     vec3 WorldPos;
     vec4 preScreenPos;
     vec4 nowScreenPos;
     vec3 Normal;
     vec2 halton;
} vs_out;

uniform mat4 model;

uniform float screenWidth;
uniform float screenHeight;
uniform int frameCount;


uniform mat4 prevModel;
uniform mat4 prevView;
uniform mat4 prevProj;


void main()
{

   vec2 Halton_2_3[8]; 

   Halton_2_3[0] =  vec2(0.0f, -1.0f / 3.0f);
   Halton_2_3[1] =  vec2(-1.0f / 2.0f, 1.0f / 3.0f);
   Halton_2_3[2] = vec2(1.0f / 2.0f, -7.0f / 9.0f);
   Halton_2_3[3] =  vec2(-3.0f / 4.0f, -1.0f / 9.0f);
   Halton_2_3[4] =  vec2(1.0f / 4.0f, 5.0f / 9.0f);
   Halton_2_3[5] =  vec2(-1.0f / 4.0f, -5.0f / 9.0f);
   Halton_2_3[6] =  vec2(3.0f / 4.0f, 1.0f / 9.0f);
   Halton_2_3[7] =  vec2(-7.0f / 8.0f, 7.0f / 9.0f);


    vs_out.TexCoords = aTexCoords;
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal =  transpose(inverse(mat3(model))) * aNormal; 
    vs_out.preScreenPos = prevProj * prevView * prevModel * vec4(aPos, 1.0);
    vs_out.nowScreenPos = u_projection * u_view * model * vec4(aPos, 1.0);
    vs_out.halton =vec2(screenWidth);

    float deltaWidth = 1.0/float(screenWidth), deltaHeight = 1.0/float(screenHeight);
    vec2 jitter = vec2(
        (Halton[frameCount % 15].x * 2.0 - 1.0) / screenWidth,
        (Halton[frameCount % 15].y * 2.0 - 1.0) / screenHeight
    );

    vec2 _jitter = vec2((1.0f / 2.0f) / screenWidth,(-7.0f / 9.0f) / screenHeight);

    mat4 jitterMat = u_projection;
//    jitterMat[2][0] += _jitter.x;
//    jitterMat[2][1] += _jitter.y;


   // gl_Position =  vec4(5.0);//0*jitterMat * u_view * vec4(vs_out.WorldPos, 1.0);
}