#version 410 core

uniform sampler2D sliceImageCT;
uniform sampler2D sliceImageMR;
uniform sampler2D electrodeImage;
uniform sampler2D topImage;

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

  /*if(electrode.w > sliceCT.w){
  	outputColor = electrode;
  }*/
  if( (sliceCT.w-electrode.w)*(sliceCT.w-electrode.w) < 1.0){
  	outputColor = electrode;
  	
  }else if(electrode.w > sliceCT.w){
  	float distFactor = length(electrode.w - sliceCT.w)/5.0f;
  	distFactor = clamp(distFactor,0,1);
  	distFactor = pow(distFactor,2);
  	outputColor = outputColor*(distFactor)+electrode*(1.0-distFactor);
  }
}

