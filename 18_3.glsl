
[Vertex Shader]
// Shader OpenGL ID 1
#version 120
#extension GL_EXT_geometry_shader4 : enable

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
[Fragment Shader]
// Shader OpenGL ID 2
#version 120
#extension GL_EXT_geometry_shader4 : enable

varying vec3 n;
varying vec3 v;    

uniform sampler2D mytext;
uniform float x;
uniform vec3 mouse;

// Global variables
vec4 final_color;

// ---------------- Phong lighting ----------------------//
void phongLighting()
{
	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0) surf2Eye  
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);   // surf2Light
	vec3 R = normalize(-reflect(L,n));  // Reflection of surf2Light and normal

	vec4 texelColor = texture2D(mytext, gl_TexCoord[0].st);

	//calculate Ambient Term:  
	//vec4 Iamb = texelColor.rgba * gl_FrontMaterial.ambient * gl_LightSource[0].ambient;    
	vec4 Iamb = gl_Color * gl_FrontMaterial.ambient * gl_LightSource[0].ambient;   

	//calculate Diffuse Term:  
	//vec4 Idiff = texelColor.rgba * gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse *
	vec4 Idiff = gl_Color * gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse *
				 max(dot(n,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);     

	// calculate Specular Term:
	vec4 Ispec =  gl_FrontMaterial.specular * gl_LightSource[0].specular * 
				  pow(max(dot(R,E),0.0), 0.3 * gl_FrontMaterial.shininess);
	Ispec = clamp(Ispec, 0.0, 1.0); 
	
	// Calculate final color:
	final_color  = Iamb + Idiff + Ispec;
	
	//final_color = texelColor;
	//final_color = vec4(mouse.x, 0, 0, 1);
}

// ---------------- Main function ----------------------//
void main (void)  
{
	phongLighting();


	
	//if ()
	//{
	//}
	gl_FragColor = final_color;
}