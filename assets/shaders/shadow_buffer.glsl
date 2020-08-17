#pragma version 420
#include "include/uniforms.glsl"
#pragma vertex
attribute vec3 v_position;
         
void main()
{
   gl_Position = viewMatrices.projection * viewMatrices.view * modelMatrix * vec4(v_position, 1.0);       
}

#pragma fragment
void main()
{
}
