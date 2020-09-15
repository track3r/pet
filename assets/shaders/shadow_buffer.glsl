#pragma version 420
#include "include/common.glsl"
#include "include/uniforms.glsl"
//flat varying uint dbg_drawId;
//varying mat4 dbg_transform_di;
//varying mat4 dbg_transform_v;
//varying vec3 dbg_pos;
#pragma vertex

attribute vec3 v_position;
attribute mat4 v_matrix;
attribute uint v_drawId;

void main()
{
   //dbg_drawId = v_drawId;
   //dbg_transform_di = instance[v_drawId].transform;
   //dbg_transform_v = v_matrix;
   //dbg_pos = v_position;
   gl_Position = viewMatrices.projection * viewMatrices.view * instance[v_drawId].transform * vec4(v_position, 1.0);
   //gl_Position = viewMatrices.projection * viewMatrices.view * v_matrix * vec4(v_position, 1.0);
   //gl_Position = viewMatrices.projection * viewMatrices.view * modelMatrix * vec4(v_position, 1.0);
   //gl_Position = viewMatrices.projection * viewMatrices.view * instance[u_drawId].transform * vec4(v_position, 1.0);
   
}

#pragma fragment
void main()
{
}
