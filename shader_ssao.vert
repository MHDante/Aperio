/*******************************************************************
Vertex
SSAO Post-process Shader : Screen Space Ambient Occlusion Shader 
*******************************************************************/

#version 440 compatibility

smooth out vec2 vUv;

void main() 
{
	vUv = gl_MultiTexCoord0.st;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
