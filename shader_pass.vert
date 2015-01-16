/*******************************************************************
	Vertex
	Simple Pass-through Shader
*******************************************************************/
#version 420 compatibility

// Output to fs
out vec3 n;
out vec3 v;
out vec3 original_v;

out vec4 vTexCoord;

//******************** Main ***************************************
void main()  
{     
   v = vec3(gl_ModelViewMatrix * gl_Vertex);       
   n = normalize(gl_NormalMatrix * gl_Normal);

   original_v = vec3(gl_Vertex);       
	
   gl_FrontColor = gl_Color;
   gl_BackColor = gl_Color;
   vTexCoord = gl_MultiTexCoord0;   
	
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}
