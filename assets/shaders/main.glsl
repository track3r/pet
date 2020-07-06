#version 420
#include "include/varyingDefault.glsl"
#include "include/uniforms.glsl"
#pragma vertex
attribute vec3 v_position;
attribute vec2 v_uv;
attribute vec3 v_normal;

//uniform mat4 v_mMatrix;
//uniform mat4 v_vMatrix;
//uniform mat4 v_pMatrix;

uniform mat4 v_lMatrix;
uniform vec3 v_lightPos;
         
void main()
{
            
    f_texcoord0 = v_uv;
    gl_Position = viewMatrices.projection * viewMatrices.view * modelMatrix * vec4(v_position, 1.0);
        
    vec3 posWorld = (modelMatrix * vec4(v_position, 1.0)).xyz;
    f_posLightspace = v_lMatrix * vec4(posWorld, 1.0);
    mat4 eyeMatrix = viewMatrices.view * modelMatrix;
            
    //mat4 eyeMatrix = modelMatrix;
    f_debug = vec4(eyeMatrix * vec4(v_position, 0.0) ).xyz; 
    vec4 eyePos = eyeMatrix * vec4(v_position, 1.0);
    f_toCamera = -eyePos.xyz;

    f_normal = vec4(eyeMatrix * vec4(v_normal, 0.0)).xyz;

    vec3 testLightDirWorld = vec3(0, 1, 0);
            
    
    vec3 lightPosEye = vec4(eyeMatrix * vec4(v_lightPos, 1.0)).xyz;
    //vec4 lightPos = (inverse(v_lMatrix) * vec4(0.5, -1, .5, 1.0));
    //lightPos /= lightPos.w;
    //lightPosEye = vec4(eyeMatrix * vec4(lightPos.xyz, 1.0)).xyz;
    f_toLight = lightPosEye - eyePos.xyz;
            
    f_lighDir = vec4(eyeMatrix * vec4(testLightDirWorld, 0.0)).xyz;
            
}

#pragma fragment
uniform sampler2D texture0;
//uniform sampler2DShadow textureShadow;
uniform sampler2D textureShadow;

float sampleShadow(vec3 projCoords, float bias)
{
    float closestDepth = texture(textureShadow, projCoords.xy).r;// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float currentDepth = projCoords.z;// get depth of current fragment from light's perspective
    return closestDepth < currentDepth - bias  ? 0.0 : 1.0;
}

void main()
{
    gl_FragColor.a = 1.0;

    vec3 normal = normalize(f_normal);
    float ndl = dot(normal,normalize(f_toLight));
    float distance = length(f_toLight);
    float att = 1.0 + distance*0.0004  + distance*distance*0.0004;
    ndl /= att;            

    vec3 projCoords = f_posLightspace.xyz / f_posLightspace.w; // perform perspective divide
    projCoords = projCoords * 0.5 + 0.5;// transform to [0,1] range
            
    //gl_FragColor.xyz = vec3(projCoords.y);
    //return;
            
    float shadow = 0.0;
    bool inShadow = projCoords.x > 0.0 && projCoords.x < 1.0 &&
        projCoords.y > 0.0 && projCoords.y < 1.0 &&
        projCoords.z > 0.0 && projCoords.z < 1.0;

    if (inShadow)
    {
                
        float closestDepth = texture(textureShadow, projCoords.xy).r;// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float currentDepth = projCoords.z;// get depth of current fragment from light's perspective
            
        // + bias gives peter panning in simple case, - bias is a mess in complex scene
        float bias = 0.0001 * ndl;                
        //shadow = closestDepth < currentDepth - bias  ? 0.1 : 1.0;

        vec2 offset = vec2(1.0/512.0);

        //shadow = sampleShadow(projCoords, bias);
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5, -1.5) * offset, 0.0), bias);
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5, -0.5) * offset, 0.0), bias);
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5,  0.5) * offset, 0.0), bias);
        shadow += sampleShadow(projCoords + vec3(vec2(-1.5,  1.5) * offset, 0.0), bias);

        shadow /= 4;
    }
            
    shadow = max(shadow, 0.1);

    //gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(vec3(ndl),1.0);
    //gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(vec3(ndl),1.0) * shadow;
            
    //ndl = 0.5;
    gl_FragColor = texture(texture0, vec2(0.5, 0.5)) * vec4(vec3(ndl * shadow) + vec3(0.2, 0.2, 0.2) ,1.0) ;
    //gl_FragColor = vec4(vec3(shadow) + vec3(0.1, 0.1, 0.1), 1.0);

    //gl_FragColor.xy = f_texcoord0;
    //gl_FragColor.a = 1.0;

    if (inShadow) {
        gl_FragColor.x += 0.5;
    }


    //gl_FragColor.y = 1;

}