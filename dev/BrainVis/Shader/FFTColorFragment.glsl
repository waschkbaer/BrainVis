#version 330 core

uniform sampler1D fftColor;

// INPUT VARIABLES
in vec3 normalview;
in vec3 entranceInViewSpace;

uniform float fftValue = 0.0f;
uniform vec2 fftRange = vec2(0.0f,5.0f);

// OUTPUT VARIABLES
layout(location=0) out vec4 outputColor;

// Pixel Shader
void main(void)
{
  float clampedFFT = (fftValue-fftRange.x)/(fftRange.y-fftRange.x);

  vec3 color =  texture(fftColor, clampedFFT).xyz;

  outputColor = vec4(color,entranceInViewSpace.z);
}
