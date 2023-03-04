#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gMetallicRoughnessAo;
layout (location = 4) out vec2 gVelo;

in VS_OUT {
    vec2 TexCoords;
    vec3 WorldPos;
    vec4 preScreenPos;
    vec4 nowScreenPos;
    vec3 Normal;
    vec2 halton;
} fs_in;


// material parameters
uniform vec3 albedo;
uniform float ao;
uniform float metallic;
uniform float roughness;


vec3 getNormalFromMap()
{
    vec3 tangentNormal = fs_in.Normal.xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.WorldPos);
    vec3 Q2  = dFdy(fs_in.WorldPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}




void main()
{    
    vec3 N = fs_in.Normal;

    gPosition = vec4(fs_in.WorldPos,1.0);
   // gPosition = vec3(1.0,0.0,0.0);
    gNormal = vec4(normalize(N),1.0);
    //gNormal = vec3(1.0,0.0,0.0);
    gAlbedo.rgba = vec4(albedo,1.0);
    //gAlbedo.rgba = vec4(1.0,0.0,0.0,1.0);
    gMetallicRoughnessAo = vec4(metallic,roughness,ao,1.0);
   // gMetallicRoughnessAo = vec3(1.0,0.0,0.0);;

       // velocity
    vec2 newPos = ((fs_in.nowScreenPos.xy / fs_in.nowScreenPos.w) * 0.5 + 0.5);
    vec2 prePos = ((fs_in.preScreenPos.xy / fs_in.preScreenPos.w) * 0.5 + 0.5);
    gVelo = newPos - prePos;
}  