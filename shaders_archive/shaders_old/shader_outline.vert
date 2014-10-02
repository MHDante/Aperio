/*******************************************************************
	Vertex
	Outline Shader 
*******************************************************************/

//#version 330 compatibility
//#version 130
//#extension GL_EXT_geometry_shader4 : enable

void main()  
{     
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}
