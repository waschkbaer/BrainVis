#version 330 core

uniform sampler2D sliceImageCT;
uniform sampler2D sliceImageMR;
uniform sampler2D electrodeImage;
uniform sampler2D CTPosition;
uniform sampler2D fontTexture;

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
  vec3 font = texture(fontTexture,vec2(vScreenPosition.x,1.0-vScreenPosition.y)).xyz;

  position -= vec3(0.5f,0.5f,0.5f);

  outputColor = sliceCT*mrctblend+sliceMR*(1.0f-mrctblend);

  if(electrode.w > sliceCT.w){
    outputColor = electrode;
  }else{
    outputColor = outputColor+0.1f*electrode;
  }

  if(electrode.x >= 0.9f &&
  electrode.y >= 0.9f &&
  electrode.z >= 0.9f){
    outputColor = +electrode*0.5f;
  }

  //if(abs(electrode.w-sliceCT.w) < 3){
  //  outputColor = electrode;
  //}

  /*if(abs(focusPoint.x-position.x) < (0.003f/zoomFactor) ){
    outputColor += vec4(0.2f,0,0,1);
  }
  if(abs(focusPoint.y-position.y) < (0.003f/zoomFactor) ){
    outputColor += vec4(0,0.2f,0,1);
  }
  if(abs(focusPoint.z-position.z) < (0.004f/zoomFactor) ){
    outputColor += vec4(0,0,0.2f,1);
  }*/

  if(!(font.x == 0 && font.y == 0 && font.z == 0)){
    outputColor = vec4(font,1);
  }
}

