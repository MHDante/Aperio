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
	vec4 Ispec = gl_FrontMaterial.specular * gl_LightSource[0].specular * 
				  pow(max(dot(R,E),0.0), 0.3 * gl_FrontMaterial.shininess);
	Ispec = clamp(Ispec, 0.0, 1.0); 
	
	// Calculate final color:
	//final_color  = Iamb + Idiff + Ispec;
	
	final_color =  vec4(vec3(gl_FrontMaterial.emission + Iamb + Idiff + Ispec), gl_Color.a);
		//final_color = vec4(gl_LightSource[0].position.z, 0, 0, 1);
	//final_color = gl_Color;
	
	//final_color = vec4(gl_FrontMaterial.ambient.r, 0, 0, 1);
	//final_color = gl_FrontMaterial.specular;
	//final_color = vec4(mouse.x, 0, 0, 1);
	//final_color = texelColor;
	//final_color = gl_FrontMaterial.emission + Iamb + Idiff + Ispec;
	//final_color = vec4(mouse.x, 0, 0, 1);
}

void toon()
{
	vec4 col = gl_Color;

	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0) surf2Eye  
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);   // surf2Light
	vec3 R = normalize(-reflect(L,n));  // Reflection of surf2Light and normal
	
	float intensity = dot(n, normalize(L));
    if(intensity < 0)
        intensity = 0;
 
    // Discretize the intensity, based on a few cutoff points
    if (intensity > 0.95)
        final_color = vec4(1.0,1,1,1.0) * col;
   // else if (intensity > 0.7)
     //   final_color = vec4(0.9,0.9,0.9,1.0) * col;
    else if (intensity > 0.5)
        final_color = vec4(0.7,0.7,0.7,1.0) * col;
   // else if (intensity > 0.25)
     //   final_color = vec4(0.5,0.5,0.5,1.0) * col;
    else if (intensity > 0.05)
        final_color = vec4(0.35,0.35,0.35,1.0) * col;
    else
        final_color = vec4(0.1,0.1,0.1,1.0) * col;
}

// ---------------- Main function ----------------------//
void main (void)  
{
	//phongLighting();
	toon();
	
	// convert mouse world coords to view coords (so same as v)
	vec4 mouseV = gl_ModelViewMatrix * vec4(vec3(mouse), 1);
	
	float ee = myexp;
	float nn = 0.5;

	float d = pow(pow(abs(v.y - mouseV.y) / 1.0, 2.0/ee) + pow(abs(v.x - mouseV.x) / 1.0, 2.0/ee), ee/nn) + pow(abs(v.z - mouseV.z) / 1.0, 2.0/nn) - 1.0;
	  
	if (selected == 1)	// if selected
	{
		if (d < mouseSize && peerInside == 1)
		{
			discard;
		}
		else
		{
			gl_FragColor = final_color;

		}
	}
	else
	{
			gl_FragColor = final_color;
	}
}