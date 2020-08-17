#pragma version 420
#include "include/common.glsl"
#include "include/uniforms.glsl"

varying vec2 f_texcoord0;
varying vec3 f_normal;
varying vec3 f_eyePos;
varying vec3 f_worldPos;

#pragma vertex
attribute vec3 v_position;
attribute vec2 v_uv;
attribute vec3 v_normal;
         
void main()
{
    gl_Position = viewMatrices.projection * viewMatrices.view * modelMatrix * vec4(v_position, 1.0);        
    f_texcoord0 = v_uv;
    f_worldPos = (modelMatrix * vec4(v_position, 1.0)).xyz;
    mat4 eyeMatrix = viewMatrices.view * modelMatrix;
    f_eyePos = (eyeMatrix * vec4(v_position, 1.0)).xyz;
    f_normal = vec4(eyeMatrix * vec4(v_normal, 0.0)).xyz;
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

void main()
{
    gl_FragColor.a = 1.0;
    mat4 eyeMatrix = viewMatrices.view * modelMatrix;
    vec3 lightPosEye = vec4(eyeMatrix * vec4(lightParams[0].pos.xyz, 1.0)).xyz;
    vec3 toLight = lightPosEye - f_eyePos.xyz;

    vec3 normal = normalize(f_normal);
    float ndl = dot(normal,normalize(toLight));
    float distance = length(toLight);

    float att = 1.0 + distance*0.0004  + distance*distance*0.0004;
    ndl /= att;            

    vec4 posLightspace = lightParams[0].matrix * vec4(f_worldPos, 1.0);
    vec3 projCoords = posLightspace.xyz / posLightspace.w; // perform perspective divide
    projCoords = projCoords * 0.5 + 0.5;// transform to [0,1] range
            
    //gl_FragColor.xyz = vec3(projCoords.y);
    //return;
            
    float shadow = 0.0;
    bool inShadow = projCoords.x > 0.0 && projCoords.x < 1.0 &&
        projCoords.y > 0.0 && projCoords.y < 1.0 &&
        projCoords.z > 0.0 && projCoords.z < 1.0;

    if (inShadow)
    {
                
        //float closestDepth = texture(textureShadow, projCoords.xy).r;// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        //float currentDepth = projCoords.z;// get depth of current fragment from light's perspective
            
        // + bias gives peter panning in simple case, - bias is a mess in complex scene
        float bias = 0.0001 * ndl;                
        //shadow = closestDepth < currentDepth - bias  ? 0.1 : 1.0;
        
        vec2 offset = vec2(1.0/512.0);
        //projCoords.z += bias;
        
        //shadow = sampleShadow(projCoords, bias);
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
        bias = 0.0;
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(-dist, -dist) * offset, 0.0), bias, 0);
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(dist, -dist) * offset, 0.0), bias, 0);
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(-dist, dist) * offset, 0.0), bias, 0);
        shadow += sampleShadowhPCF(projCoords + vec3(vec2(dist, dist) * offset, 0.0), bias, 0);
        shadow /= 4;
    }
            
    shadow = max(shadow, 0.1);
    //shadow = 1;

    gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(vec3(ndl * shadow) + vec3(0.2, 0.2, 0.2) ,1.0) ;
    //gl_FragColor = vec4(vec3(shadow) + vec3(0.1, 0.1, 0.1), 1.0);

    //gl_FragColor.xy = f_texcoord0;
    //gl_FragColor.a = 1.0;

    if (inShadow) {
        //gl_FragColor.x += 0.5;
    }


    //gl_FragColor.y = 1;

}