#version 420 core

layout(binding=0) uniform sampler2D sliceImageCT;
layout(binding=1) uniform sampler2D sliceImageMR;
layout(binding=2) uniform sampler2D electrodeImage;
layout(binding=3) uniform sampler2D topImage;

uniform float slide = 0.0f;
uniform int axis = 0;
uniform float mrctblend = 0.5f;

// INPUT VARIABLES
in vec2 vScreenPosition;

// OUTPUT VARIABLES
out vec4 outputColor;

// Pixel Shader
void main(void)
{
  vec4 sliceCT = texture(sliceImageCT,vScreenPosition);
  vec4 sliceMR = texture(sliceImageMR,vScreenPosition);
  vec4 electrode = texture(electrodeImage,vScreenPosition);
  vec4 top = texture(topImage,vScreenPosition);

  outputColor = sliceCT*mrctblend+sliceMR*(1.0f-mrctblend);
}

