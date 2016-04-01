#version 330 core

// INPUT VARIABLES
layout (location=0) in vec3 inputPosition;

// OUTPUT VARIABLES
out vec3 vScreenPosition;

// UNIFORM VARIABLES
uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
uniform float slide = 0.0f;

// Vertex Shader
void main(void)
{
  vec4 pos = world*vec4(inputPosition.xyz, 1.0);

  vScreenPosition = vec3( (inputPosition.xy + vec2(1.0,1.0))/vec2(2.0,2.0) ,slide);
  //vScreenPosition = vec3( pos.xyz + vec3(0.5,0.5,0.5));

  gl_Position = projection*pos;

 
}
