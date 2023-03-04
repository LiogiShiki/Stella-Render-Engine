#version 430 core
//out vec4 FragColor;
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughnessAo;
uniform sampler2D gVelocity;
uniform sampler2D shadowMap;
uniform sampler2D HizMap;

// RSM Gbuffer
uniform sampler2D rsmPosition;
uniform sampler2D rsmNormal;
uniform sampler2D rsmFlux;

//IBL
uniform samplerCube irradianceMap;
uniform samplerCube specularEnvmap;
uniform sampler2D m_BRDFlut;

//SSR
#define INIT_STEP 0.4
#define MAX_STEPS 200
#define EPS 1e-2
#define THRES 0.1
#define INV_PI 0.31830988618
#define MAX_THICKNESS 0.0017
uniform int maxLevel;


//uniform sampler2D rsmDepth;
// RSM sample
#define SAMPLE_NUMBER 50
layout(std140) uniform randomMap
{
	vec4 randomArray[SAMPLE_NUMBER];
};

// CSM
uniform sampler2DArray CSM;
uniform float cascadePlaneDistances[16];
uniform int cascadeCount;  
uniform float far;
layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform mat4 lightSpaceMatrixRSM;


uniform mat4 lightSpaceMatrix;
uniform mat4 model;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// lights
//uniform vec3 lightPositions[4];
//uniform vec3 lightColors[4];

uniform vec3 lightDirections[4];
uniform vec3 lightStrengths[4];

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
const float PI = 3.14159265359;


vec3 GetScreenCoord(vec3  Point)
{
	vec4 positionInScreen = projectionMatrix*vec4(Point,1.0);
	positionInScreen.xyzw /= positionInScreen.w;
	positionInScreen .xyz = positionInScreen.xyz*0.5+0.5;
    return positionInScreen.xyz;
}

vec2 getCellCount(int level)
{
    return textureSize(HizMap,level);
}

vec2 getCell(vec2 pos,vec2 startCellCount)
{
 return vec2(floor(pos*startCellCount));
}

vec3 intersectDepthPlane(vec3 o, vec3 d, float t)
{
    return o + d * t;
}

float getMinimumDepthPlane(vec2 pos , int level){

    return textureLod(HizMap,pos,level).r;
}

vec3 intersectCellBoundary(vec3 o,vec3  d, vec2 rayCell,vec2 cell_count, vec2 crossStep, vec2 crossOffset)
{

    vec2 nextPos = rayCell + crossStep ;
    nextPos = nextPos/cell_count;
    nextPos = nextPos+crossOffset;

    vec2 dis  = nextPos - o.xy;

    vec2 delta = dis/d.xy;

    float t = min(delta.x,delta.y);

    return intersectDepthPlane(o,d,t);
}

bool crossedCellBoundary(vec2 oldCellIdx,vec2 newCellIdx){
    return (oldCellIdx.x!=newCellIdx.x)||(oldCellIdx.y!=newCellIdx.y);
}
vec3 FindIntersection(vec3 start,vec3 rayDir,float maxTraceDistance, vec3 hitPos){

    vec2 crossStep = vec2(rayDir.x>=0?1:-1,rayDir.y>=0?1:-1);
    vec2 crossOffset = crossStep / vec2(1024.0,1024.0) / 128;
    crossStep = clamp(crossStep,0.0,1.0);

    vec3 ray = start;
    float minZ = ray.z;
    float maxZ = ray.z+rayDir.z*maxTraceDistance;
    float deltaZ = (maxZ-minZ);

    vec3 o = ray;
    vec3 d = rayDir*maxTraceDistance;

    int startLevel = 2;
    int stopLevel = 0;
    vec2 startCellCount = getCellCount(startLevel);


    vec2 rayCell = getCell(ray.xy,startCellCount);
    ray = intersectCellBoundary(o, d, rayCell, startCellCount, crossStep, crossOffset*64);

    int level = startLevel;
    int iter = 0;
    bool isBackwardRay = rayDir.z<0;

    float Dir = isBackwardRay ? -1 : 1;

    while( level>=stopLevel && ray.z*Dir <= maxZ*Dir && iter<100){
        vec2 cellCount = getCellCount(level);
        vec2 oldCellIdx = getCell(ray.xy,cellCount);

        float cell_minZ = getMinimumDepthPlane(ray.xy, level);

        vec3 tmpRay = ((cell_minZ>ray.z) && !isBackwardRay) ? intersectDepthPlane(o,d,(cell_minZ-minZ)/deltaZ) :ray;

        vec2 newCellIdx = getCell(tmpRay.xy,cellCount);

        float thickness = level ==0 ? (ray.z-cell_minZ) : 0;
        bool crossed  = (isBackwardRay&&(cell_minZ>ray.z))||(thickness>MAX_THICKNESS)|| crossedCellBoundary(oldCellIdx, newCellIdx);

        ray = crossed ? intersectCellBoundary(o, d, oldCellIdx, cellCount, crossStep, crossOffset):tmpRay;
        level = crossed ? min(maxLevel,level+1):level-1;
        ++iter;

    }
    bool intersected = (level < stopLevel);
    hitPos = ray;

    return intersected ? texture(gAlbedo,hitPos.xy).rgb:vec3(0.0);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float LinearizeDepth(vec2 uv)
{
  float n = 0.1;		// camera z near
  float f = 400.0;		// camera z far
  float z = textureLod(HizMap, uv, 1).x;
  return (2.0 * n) / (f + n - z * (f - n));
}


float ShadowCalculationCSM(vec3 fragPosWorldSpace,vec3 fragNoraml)
{
    vec4 fragPosViewSpace = viewMatrix * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    currentDepth = clamp(currentDepth,0.0,1.0);
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fragNoraml);
    vec3 lightDir = normalize(-lightDirections[0]);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (far * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(CSM, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(CSM, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace,vec3 fragNoraml)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    //vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-lightDirections[0]);
    float bias = max(0.05 * (1.0 - dot(fragNoraml, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 Sample_RSM(vec4 FragPosLightSpaceRSM,vec3 FragWorldPos,vec3 FragNormal)
{
    vec3 color_indir = vec3(1.0);
    vec3 ProjectCoordRSM = FragPosLightSpaceRSM.xyz / FragPosLightSpaceRSM.w;
    ProjectCoordRSM = ProjectCoordRSM*0.5 + 0.5;

    
    float filterSize = 1024;
    float filterRange = 25;
    for(int i=0;i<SAMPLE_NUMBER;i++)
    {
 
	vec2 uv = ProjectCoordRSM.xy+randomArray[i].xy*(filterRange/filterSize);

	vec3 irrandance = texture(rsmFlux,uv).rgb;
	vec3 idir_p = texture(rsmPosition,uv).rgb;
	vec3 idir_wi = normalize(texture(rsmPosition,uv).rgb - FragWorldPos);
	vec3 idir_normal = normalize(texture(rsmNormal,uv).rgb);
	vec3 sample_color = (irrandance*max(dot(idir_wi,FragNormal),0)*max(dot(idir_normal,FragWorldPos-idir_p),0)/pow(length(idir_p-FragWorldPos),4))*randomArray[i].z;
	//vec3 sample_color = irrandance;
	color_indir+=sample_color;
 
    }
    color_indir=clamp(color_indir/SAMPLE_NUMBER, 0.0, 1.0);


    return color_indir;
}


void main()
{    
    vec3 WorldPos = texture(gPosition,TexCoords).rgb;
    vec3 normal = texture(gNormal,TexCoords).rgb;
    vec4 albedo = texture(gAlbedo,TexCoords);
    vec3 metallicRouhenessAo = texture(gMetallicRoughnessAo,TexCoords).rgb;
    vec2 velocity = texture(gVelocity,TexCoords).rg;

    // Prepare for SSR
    vec3 ViewPos = vec3(viewMatrix * vec4(WorldPos,1.0));
    vec3 ViewNormal = vec3(mat3(viewMatrix) * normal);
    vec3 relfectDir = normalize(reflect(ViewPos,ViewNormal)); // cause in view space ,camera pos is (0,0,0)
    vec3 endPosInView = ViewPos+relfectDir*3000;

    vec3 start = GetScreenCoord(ViewPos); // start is in screen coord
    vec3 end = GetScreenCoord(endPosInView); // end is in screen coord
    vec3 rayDir = normalize(end-start);

    float maxTraceX = rayDir.x>=0 ? (1-start.x)/rayDir.x : -start.x/rayDir.x; // for screen coord, 1 is the max
    float maxTraceY = rayDir.y>=0 ? (1-start.y)/rayDir.y : -start.y/rayDir.y;
    float maxTraceZ = rayDir.z>=0 ? (1-start.z)/rayDir.z : -start.z/rayDir.z;
    // olny trace inside the min of max tracing distance
    float maxTraceDistance = min(maxTraceX,min(maxTraceY,maxTraceZ)); 

    // Do SSR
    vec3 hitPos = vec3(0.0);

    vec3 L_indir = FindIntersection(start,rayDir,maxTraceDistance,hitPos)*texture(gAlbedo,TexCoords).rgb*INV_PI;


    float metallic = metallicRouhenessAo.x;
    float roughness = metallicRouhenessAo.y;
    float ao        = metallicRouhenessAo.z;

    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - WorldPos);
    vec3 R = reflect(-V, N); 


    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo.xyz, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        //vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 L = normalize(-lightDirections[i]);
        vec3 H = normalize(V + L);
        float distance = 10.0f; // hardcode distance;
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightStrengths[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo.xyz / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   


    // ambient IBL
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness); 
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo.rgb;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(specularEnvmap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(m_BRDFlut, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    // shadow

    vec4 FragPosLightSpace = lightSpaceMatrix  * vec4(WorldPos,1.0);
   // float shadow = ShadowCalculation(FragPosLightSpace,normal);                      
    float shadow = ShadowCalculationCSM(WorldPos,normal);                      
   
   // RSM
    vec4 FragPosLightSpaceRSM = lightSpaceMatrixRSM  * vec4(WorldPos,1.0);
    vec3 color_indir_rsm = Sample_RSM(FragPosLightSpaceRSM,WorldPos,normal);
   

    if(ao<0.005)
    {
        ao = 0.8;
    }
    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = (0.0*ambient + (1-shadow) *Lo);
    ;//(1.0*ambient + (1-shadow) *Lo);
  //  vec3 color =  (ambient + Lo);

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 


    float s = texture(shadowMap, TexCoords.xy).r; 
   // float depth = LinearizeDepth(TexCoords.xy);


//   float4 fragSimpleReinhard(v2f i) : SV_Target
    //
    //{
    //float4 texColor = tex2D(_MainTex, i.uv);
    //float lum = Luminance(texColor.rgb); 
    //float lumTm = lum * _ExposureAdjustment;//ÆØ¹âÏµÊý
    //float scale = lumTm / (1+lumTm);  
    //return float4(texColor.rgb * scale / lum, texColor.a);
    //}

    //float d = LinearizeDepth(TexCoords);

    vec4 fragPosViewSpace = viewMatrix * vec4(WorldPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    const vec3 layerColor[6] = 
    {
    vec3(1.0,0.0,0.0),
    vec3(0.0,1.0,0.0),
    vec3(0.0,0.0,1.0),
    vec3(0.0,0.0,0.0),
    vec3(1.0,1.0,1.0),
    vec3(1.0,1.0,0.0)};

    //color = layerColor[layer];
    FragColor = vec4(color + 0*L_indir , 1.0);// vec4(vec3(color + color_indir_rsm *4), 1.0);
  
}
