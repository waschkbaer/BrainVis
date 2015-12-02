#version 410 core

uniform sampler3D volume;
uniform sampler1D transferfunction;

uniform int axis = 0;
uniform float tfScaling;

// INPUT VARIABLES
in vec3 vScreenPosition;
in vec3 vViewPos;

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
  outputColor = vec4(value,value,value,1);
  outputColor.w = vViewPos.z;
  outputPosition = vec4(coord,1);

  if( coord.x > 1.0 || coord.x < 0.0 || 
      coord.y > 1.0 || coord.y < 0.0 || 
      coord.z > 1.0 || coord.z < 0.0){
      outputColor = vec4(0,0,0,-20000);
  }
}

