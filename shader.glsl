#version 130
//#extension GL_EXT_geometry_shader4 : enable

varying vec3 n;
varying vec3 v;

void main(void)  
{     
   v = vec3(gl_ModelViewMatrix * gl_Vertex);       
   n = normalize(gl_NormalMatrix * gl_Normal);

   //original_v = vec3(gl_Vertex);         
   
   gl_FrontColor = gl_Color;
   gl_BackColor = gl_Color;
   gl_TexCoord[0] = gl_MultiTexCoord0;   
	
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}