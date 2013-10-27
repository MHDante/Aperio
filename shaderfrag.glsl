varying vec3 n;
varying vec3 v;    

uniform vec3 mouse;
uniform float mouseSize;
uniform int selected;
uniform int peerInside;

uniform float myexp;

uniform sampler2D mytext;

// Global variables
vec4 final_color;

// ---------------- Phong lighting ----------------------//
void phongLighting()
{
	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)  
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
	vec3 R = normalize(-reflect(L,n));  

	//calculate Ambient Term:  
	vec4 Iamb = gl_FrontLightProduct[0].ambient;    

	//calculate Diffuse Term:  
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(n,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);     

	// calculate Specular Term:
	vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
	Ispec = clamp(Ispec, 0.0, 1.0); 

	vec4 texelColor = texture2D(mytext, gl_TexCoord[0].st);
	
	// Calculate final color:
	final_color  = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff * texelColor + Ispec;
}

// ---------------- Main function ----------------------//
void main (void)  
{  
	phongLighting();

	// convert mouse world coords to view coords (so same as v)
	vec4 mouseV = gl_ModelViewMatrix * vec4(vec3(mouse), 1);
	
	float ee = myexp;
	float nn = 0.5;

	float d = pow(pow(abs(v.y - mouseV.y) / 1.0, 2.0/ee) + pow(abs(v.x - mouseV.x) / 1.0, 2.0/ee), ee/nn) + pow(abs(v.z - mouseV.z) / 1.0, 2.0/nn) - 1.0;
	  
	if (selected == 1)	// if selected
	{
		if (d < mouseSize)
		{
			discard;
		}
		else
		{
			//vec4 new_color = vec4(final_color) + vec4(.4, 0, 0, 0);
			//gl_FragColor = vec4(vec3(new_color), gl_Color.a);     
			gl_FragColor = final_color;

		}
	}
	else
	{
		//gl_FragColor = vec4(vec3(final_color), gl_Color.a);     
			gl_FragColor = final_color;
	}
	//gl_FragColor = texture2D(mytext, gl_TexCoord[0].st) *.8 + final_color*.2; 
}