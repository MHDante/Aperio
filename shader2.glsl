#version 120
//#extension GL_EXT_geometry_shader4 : enable

void main(void)  
{     
   gl_TexCoord[0] = gl_MultiTexCoord0;   
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}