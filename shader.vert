/*******************************************************************
	Vertex
	Main Shader : Phong-Minneart Reflection model and main interaction	
*******************************************************************/

//#version 130
//#extension GL_EXT_geometry_shader4 : enable

varying vec3 n;
varying vec3 v;
varying vec3 original_v;

varying float vertexID;

void main()  
{     
   v = vec3(gl_ModelViewMatrix * gl_Vertex);       
   n = normalize(gl_NormalMatrix * gl_Normal);

   original_v = vec3(gl_Vertex);       
	
   gl_FrontColor = gl_Color;
   gl_BackColor = gl_Color;
   gl_TexCoord[0] = gl_MultiTexCoord0;   
	
   vertexID = (float)gl_VertexID;
	
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}
