#version 410 core

uniform sampler3D volume;
uniform sampler1D transferfunction;

uniform int axis = 0;
uniform float tfScaling;

uniform vec3 target1 = vec3(0,0,0);
uniform vec3 target2 = vec3(0,0,0);
uniform vec3 entry1 = vec3(0,0,0);
uniform vec3 entry2 = vec3(0,0,0);
uniform float radius = 0.01f;

// INPUT VARIABLES
in vec3 vScreenPosition;

// OUTPUT VARIABLES
layout(location=0) out vec4 outputColor;
layout(location=1) out vec4 outputPosition;

// Pixel Shader
void main(void)
{
  float value = 0.0f;

  vec3 coord = vScreenPosition;
  
  value = texture(volume,coord).x;
  value *= tfScaling;

  outputColor = texture(transferfunction, value);
  outputPosition = vec4(coord,1);

 // outputColor = vec4(coord,1);
}

