#pragma version 420
//how to fetch model matrix
//#pragma permute P_DRAWID,P_UMODEL, P_DIVISOR
#define P_DRAWID
//#define P_VDRAWID
#include "include/common.glsl"
#include "include/uniforms.glsl"

varying vec2 f_texcoord0;
varying vec3 f_normal;
varying vec3 f_eyePos;
varying vec3 f_worldPos;

#if defined(P_VDRAWID)
    #if defined(vertex)
attribute uint v_drawId;
    #endif
flat varying uint f_drawId;
#endif


mat4 getInstanceMatrix()
{
#if defined (P_DRAWID)
    return instance[u_drawId].transform;
#elif defined (P_UMODEL)
    return modelMatrix;
#elif defined(P_VDRAWID)
    #if defined(vertex)
        return instance[v_drawId].transform;
    #else
        return instance[f_drawId].transform;
    #endif
#endif
}

#pragma vertex
attribute vec3 v_position;
attribute vec2 v_uv;
attribute vec3 v_normal;

void main()
{
    mat4 instanceMatrix = getInstanceMatrix();
    gl_Position = viewMatrices.projection * viewMatrices.view * instanceMatrix * vec4(v_position, 1.0);        
    f_texcoord0 = v_uv;
    f_worldPos = (instanceMatrix * vec4(v_position, 1.0)).xyz;
    mat4 eyeMatrix = viewMatrices.view * instanceMatrix;
    f_eyePos = (eyeMatrix * vec4(v_position, 1.0)).xyz;
    f_normal = vec4(eyeMatrix * vec4(v_normal, 0.0)).xyz;
    #if defined(P_VDRAWID)
        f_drawId = v_drawId;
    #endif
}

#pragma fragment
uniform sampler2D texture0;
uniform sampler2DShadow textureShadow;
//uniform sampler2D textureShadow;

#if 0
float sampleShadow(vec3 projCoords, float bias, int light)
{
    vec4 atlas = lightParams[light].atlas;

    vec2 finalCoord = mix(atlas.xy, atlas.zw, projCoords.xy);
    float closestDepth = texture(textureShadow, finalCoord).r;// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float currentDepth = projCoords.z;// get depth of current fragment from light's perspective
    //return closestDepth < currentDepth - bias  ? 0.0 : 1.0;
    return closestDepth + bias < currentDepth  ? 0.0 : 1.0;
}
#endif

float sampleShadowhPCF(vec3 projCoords, float bias, int light)
{
    vec4 atlas = lightParams[light].atlas;
    vec3 finalCoord = vec3(mix(atlas.xy, atlas.zw, projCoords.xy), projCoords.z + bias);
    return texture(textureShadow, finalCoord);
}

vec3 calculateLight(int lightId, mat4 eyeMatrix, vec3 normal)
{
    vec3 lightPosEye = vec4(eyeMatrix * vec4(lightParams[lightId].pos.xyz, 1.0)).xyz;
    vec3 toLight = lightPosEye - f_eyePos.xyz;
    float ndl = clamp(dot(normal,normalize(toLight)), 0.0, 1.0);
    float distance = length(toLight);
    float att = 1.0 + distance*0.0004  + distance*distance*0.0004;
    if ( ndl < 0.01)
    {
        return vec3(0.0);
    }
    ndl /= att;
    vec4 posLightspace = lightParams[lightId].matrix * vec4(f_worldPos, 1.0);
    vec3 projCoords = posLightspace.xyz / posLightspace.w; // perform perspective divide
    projCoords = projCoords * 0.5 + 0.5;// transform to [0,1] range
    float shadow = 0.0;
    bool inShadow = projCoords.x > 0.0 && projCoords.x < 1.0 &&
        projCoords.y > 0.0 && projCoords.y < 1.0 &&
        projCoords.z > 0.0 && projCoords.z < 1.0;

    if (inShadow)
    {
            
        // + bias gives peter panning in simple case, - bias is a mess in complex scene
        //float bias = 0.0001 * ndl;                
        float bias = 0.005f*tan(acos(ndl));
        bias = clamp(bias, 0.0f,0.02f);
        vec2 offset = vec2(1.0/512.0);
        
        #if 0//manual
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5, -1.5) * offset, 0.0), bias, 0);
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5, -0.5) * offset, 0.0), bias, 0);
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5,  0.5) * offset, 0.0), bias, 0);
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5,  1.5) * offset, 0.0), bias, 0);
        shadow /= 4;
        #endif

        #if 0//hardware pcf
        vec4 atlas = lightParams[0].atlas;
        vec3 finalCoord = vec3(mix(atlas.xy, atlas.zw, projCoords.xy), projCoords.z + bias);
        shadow = texture(textureShadow, finalCoord);
        #endif

        //hardware + manual pcf
        float dist = 1.0;
        bias = 0.0;//no acne ether way
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(-dist, -dist) * offset, 0.0), bias, lightId);
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(dist, -dist) * offset, 0.0), bias, lightId);
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(-dist, dist) * offset, 0.0), bias, lightId);
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(dist, dist) * offset, 0.0), bias, lightId);
        shadow /= 4;
    }
    else
    {
        return vec3(0.0);
    }
            
    shadow = max(shadow, 0.1);

    return vec3(ndl * shadow);
}

void main()
{
    gl_FragColor.a = 1.0;
    mat4 instanceMatrix = getInstanceMatrix();
    mat4 eyeMatrix = viewMatrices.view * instanceMatrix;
    vec3 normal = normalize(f_normal);

    //shadow = 1;
    vec3 lightSum = vec3(0.0);
    for (int i = 0; i < numLights; i++)
    {
        vec3 light = calculateLight(i, eyeMatrix, normal);
        lightSum += light;
    }
    
    vec3 ambient = vec3(0.02, 0.02, 0.02);
    //f_texcoord0
    //gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(lightSum + ambient, 1.0) ;
    gl_FragColor = texture(texture0, f_texcoord0) * vec4(lightSum + ambient, 1.0) ;

    //if (inShadow) {
        //gl_FragColor.x += 0.5;
    //}
}