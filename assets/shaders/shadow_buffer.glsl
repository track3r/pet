#pragma version 420
#include "include/uniforms.glsl"
#pragma vertex
attribute vec3 v_position;
attribute mat4 v_matrix;
void main()
{
   gl_Position = viewMatrices.projection * viewMatrices.view * v_matrix * vec4(v_position, 1.0);       
}

#pragma fragment
void main()
{
}
