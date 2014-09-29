/*******************************************************************
	Vertex
	Pre-pass Shader : Run on vertex data
*******************************************************************/

#version 450 compatibility

smooth out vec3 n;
smooth out vec3 v;
smooth out vec3 original_v;

void main()  
{     
   	v = vec3(gl_ModelViewMatrix * gl_Vertex);
    n = normalize(gl_NormalMatrix * gl_Normal);

	original_v = gl_Vertex.xyz;
	
	gl_FrontColor = gl_Color;
	gl_BackColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec4 final_position = gl_ModelViewProjectionMatrix * (gl_Vertex);
	gl_Position = final_position ;
}
