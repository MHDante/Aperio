/*******************************************************************
	Vertex
	FXAA Post-process Shader : FXAA Fast Approximate Anti-Aliasing 
*******************************************************************/

//uniform vec2 frameBufSize;
varying vec4 posPos;
//float FXAA_SUBPIX_SHIFT = 1.0/4.0;

void main(void)
{
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  
  gl_TexCoord[0] = gl_MultiTexCoord0;
  
  //vec2 rcpFrame = vec2(1.0/frameBufSize.x, 1.0/frameBufSize.y);
  posPos.xy = gl_MultiTexCoord0.xy;
  //posPos.zw = gl_MultiTexCoord0.xy - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}