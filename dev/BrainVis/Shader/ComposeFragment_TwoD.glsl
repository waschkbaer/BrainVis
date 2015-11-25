#version 410 core

uniform sampler2D sliceImageCT;
uniform sampler2D sliceImageMR;
uniform sampler2D electrodeImage;
uniform sampler2D CTPosition;
uniform sampler2D topImage;

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
  vec4 electrode = texture(electrodeImage,vScreenPosition);
  vec3 position = texture(CTPosition,vScreenPosition).xyz;
  vec4 top = texture(topImage,vScreenPosition);

  position -= vec3(0.5f,0.5f,0.5f);

  outputColor = sliceCT*mrctblend+sliceMR*(1.0f-mrctblend);

  if( abs(sliceCT.w-electrode.w) < 10.0){
  	outputColor = electrode;
  	
  }else if(electrode.w > sliceCT.w){
  	outputColor = outputColor+ 0.1*electrode;
  }

  if(abs(focusPoint.x-position.x) < (0.003f/zoomFactor) ){
    outputColor += vec4(0.5f,0,0,1);
  }
  if(abs(focusPoint.y-position.y) < (0.003f/zoomFactor) ){
    outputColor += vec4(0,0.5f,0,1);
  }
  if(abs(focusPoint.z-position.z) < (0.004f/zoomFactor) ){
    outputColor += vec4(0,0,0.5f,1);
  }


}

