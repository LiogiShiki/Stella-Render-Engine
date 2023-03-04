#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gFlux;

in VS_OUT {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
} fs_in;


// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalsMap;
uniform vec3 lightDirections[4];

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalsMap, fs_in.TexCoords).xyz * 2.0 - 1.0;

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
  //  vec4 albedo = pow(texture(albedoMap, fs_in.TexCoords).rgba,vec4(2.2));


    vec3 wi = normalize(-lightDirections[0]);

    vec4 albedo = texture(albedoMap, fs_in.TexCoords).rgba;
    vec3 N = getNormalFromMap();

    gPosition = vec4(fs_in.WorldPos,1.0);
   // gPosition = vec3(1.0,0.0,0.0);
    gNormal = vec4(normalize(N),1.0);
    //gNormal = vec3(1.0,0.0,0.0);
    gFlux.rgba = max(dot(N,wi),0.02)*albedo*2.0;
   // gFlux.rgba = albedo;

}  