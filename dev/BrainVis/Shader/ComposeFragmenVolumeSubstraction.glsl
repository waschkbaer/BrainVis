#version 410 core

uniform sampler2D sliceImageCT;
uniform sampler2D sliceImageMR;

uniform int axis = 0;
uniform float mrctblend = 0.5f;
uniform vec3 focusPoint = vec3(0.0f,0.0f,0.0f);
uniform float zoomFactor = 1.0f;

// INPUT VARIABLES
in vec2 vScreenPosition;

// OUTPUT VARIABLES
out vec4 outputColor;

// Pixel Shader
void main(void)
{
  vec4 sliceCT = texture(sliceImageCT,vScreenPosition);
  vec4 sliceMR = texture(sliceImageMR,vScreenPosition);
 
  outputColor = sliceCT-sliceMR;
}

