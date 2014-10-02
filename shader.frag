/*******************************************************************
Fragment
Main Shader : Phong-Minneart Reflection model and main interaction
*******************************************************************/

#version 450 compatibility

uniform vec2 frameBufSize = vec2(800, 600);

smooth in vec3 n;
smooth in vec3 v;
smooth in vec3 original_v;

// Shader uniforms
uniform sampler2D depthSelectedF;
uniform sampler2D depthSelected;
uniform sampler2D depthSQ;
uniform bool cap = true;

uniform bool outline = false;
uniform bool iselem = false;

uniform bool selected = false;
uniform bool previewer = true;
uniform bool difftrans = true;

uniform vec3 mouse = vec3(0, 0, 0);
uniform float mouseSize = 1.0;
uniform float brushSize = 1.5;

uniform int shadingnum = 0;
uniform int shininess = 128;
uniform float darkness = 1.0;

uniform float myexp = 1.0;
uniform float time = 0.0;

// Superquad data
uniform vec3 pos1 = vec3(0, 0, 0);
uniform vec3 pos2 = vec3(0, 0, 0);
uniform vec3 norm1 = vec3(0, 0, 0);
uniform vec3 norm2 = vec3(0, 0, 0);
uniform vec3 scale = vec3(0, 0, 0);

uniform float phi = 0.0;
uniform float theta = 0.0;
uniform float thickness = 0.4;
uniform bool toroid = false;

uniform int elemssize;

// Light parameters
const vec4 light_ambient = vec4(0.2, 0.2, 0.2, 1);
const vec4 light_diffuse = vec4(0.6, 0.6, 0.6, 1);
const vec4 light_specular = vec4(1.0, 1.0, 1.0, 1);
const vec3 light_position = normalize(vec3(-0.080999853,6.4752009809,2.6762204566));

// Global variables
vec4 final_color = vec4(1, 1, 1, 1);

vec3 E = normalize(-v); // in Eye coords, so EyePos is (0,0,0) surf2Eye  	
vec3 L = light_position;
vec3 R = normalize(-reflect(L, n));  // Reflection of surf2Light and normal
vec3 h = normalize(E + L);

const float PI = 3.141592653;

//---Minnaert limb darkening diffuse term
vec3 minnaert(vec3 L, vec3 n, float k, vec3 light_color) 
{
	float ndotl = max(0.0, dot(L, n));
	return  light_color * pow(ndotl, k);
}

//---------------- Phong lighting (Directional) ----------------------//
void phongLighting(vec3 n, int shininess)
{
	//calculate Ambient Term:    
	vec4 theamb = gl_FrontMaterial.ambient;
	theamb.b /= 1.4;
	vec4 Iamb = (theamb * light_ambient * 1.25);

	//--- Computation of forward scattered translucency: 
	vec3 _DiffuseTranslucentColor = vec3(1,1,1);

	float att = 0.8;
	vec3 diffuseTranslucency = att * light_specular.rgb
		* vec3(_DiffuseTranslucentColor)
		* max(0.0, dot(L + vec3(0.0, -0.0, 0.0), -n));

	//--- Minnaert for darker diffuse (moon shading)
	float roughness = darkness; // minnaert roughness  1.5 default (1.0 is lambert)
	vec4 Idiff = vec4(0, 0, 0, 0);	
	
	vec3 light_color = light_diffuse.rgb;
	Idiff += vec4(minnaert(L, n, roughness, light_color), 0);
	Idiff = vec4(Idiff.rgb * gl_FrontMaterial.diffuse.rgb, 1.0);	

	vec3 light_color2 = vec3(0.325f, 0.035f, 0.0f);
	vec3 L2 = normalize(vec3(1, 1, 0.4));
	Idiff += vec4(minnaert(L2, n, roughness, light_color2), 0);
	
	Idiff = clamp(Idiff, 0.0, 1.0);
	
	//--- Physically based shader for specular lighting (energy conservation - normalization)

	float normalisation_term = (shininess + 2.0f) / 2.0f * PI;
	float blinn_phong = pow(dot(R, E), shininess);    // n_dot_h is the saturated dot product of the normal and half vectors
	float specular_term = normalisation_term * blinn_phong;

	float cosine_term = dot(n, L);
	
	float base = 1.0f - dot(h, L);    // Dot product of half vector and light vector. No need to saturate as it can't go above 90 degrees
	float exponential = pow(base, 5.0f);
	float specular_colour = 0.0025;
	
	float fresnel_term = specular_colour + (1.0f - specular_colour) * exponential;
	
	float alpha = 1.0f / (sqrt((PI / 4.0f) * shininess + (PI / 2.0f)));
	float visibility_term = (dot(n, L) * (1.0f - alpha) + alpha) * (dot(n, E) * (1.0f - alpha) + alpha); // Both dot products should be saturated
	visibility_term = 1.0f / visibility_term;

	vec3 myspecular = (PI / 4.0f) * specular_term * cosine_term * fresnel_term * visibility_term * light_specular.rgb;
	myspecular = clamp(myspecular, 0, 1);

	//--- Final color	
	vec3 tex = texture2D(depthSelectedF, gl_TexCoord[0].st * 1).rgb;	
	
	final_color = vec4(myspecular +  int(difftrans) * diffuseTranslucency + 1.0*Idiff.xyz + 0.0*tex + Iamb.xyz, gl_Color.a);
	
	if (iselem)
	{
		vec3 tex = texture2D(depthSelectedF, gl_TexCoord[0].st).rgb ;
		
		tex = vec3(1, 1, 1);
		
		// different opacities for front and back faces
		float op = 0;
		if (gl_FrontFacing)
			op = 0.1;
		else
			op = 0.2;
		
		final_color = vec4(		
		1*myspecular +  0.8* int(difftrans) * diffuseTranslucency + 1.0 * (
		(Idiff.rgb + vec3(0.35,0.35,0.35)) * tex ) - 0.0 * Iamb.xyz
		, op) ;
		//, 0.2);		
	}
	
	if (cap && iselem && !gl_FrontFacing)
	{
		vec2 texpos = vec2(gl_FragCoord.x / frameBufSize.x, gl_FragCoord.y / frameBufSize.y);	

		vec4 d_selectedF = texture2D(depthSelectedF, texpos);
		vec4 d_selected = texture2D(depthSelected, texpos);
		vec4 d_SQ = texture2D(depthSQ, texpos);
	
		// If front face discarded (empty hole) BUT
		// also backface must be present AND
		// superquad's back is closer than back of selec
		
		if (d_selectedF.a <= 0 && d_selected.a > 0
		&& d_SQ.r <= d_selected.r)
		{
		final_color = vec4(		
			1*myspecular +  0.8* int(difftrans) * diffuseTranslucency + 1.0 * (
		(Idiff.rgb + vec3(0.1, 0, 0) - 0*vec3(0.1,0.1,0.1))) - 0.0 * 	Iamb.xyz
			,1.0);
			
			// Capping Mask for SSAO shader
			gl_FragData[2] = vec4(1, 1, 1, 1);
		}
	}
	
	vec2 texpos = vec2(gl_FragCoord.x / frameBufSize.x, gl_FragCoord.y / frameBufSize.y);	
	//final_color = texture2D(depthSQ, texpos);
	
	
}

// ---------------- Toon Shader ----------------------//
void toon(vec3 n)
{
	const int levels = 5;
	const float scaleFactor = 1.0 / levels;

	vec4 col = gl_Color;

	float intensity = max(dot(n, normalize(L)), 0.0);
	vec3 diffuse = gl_Color.rgb * floor(intensity * levels) * scaleFactor;
	final_color = gl_Color / 20.0f + vec4(diffuse, gl_Color.a);
}

// ----- Superquad code
void superquad()
{
	float dist = distance(pos1, pos2);
	vec3 poss = original_v - (pos1 + pos2) / 2.0;
		
	// Rotation matrix
	vec3 right = normalize(pos2 - pos1);	
	//vec3 forward = normalize((norm1 + norm2) / 2.0);
	//vec3 up = normalize(cross(forward, right));

	vec3 up = normalize((norm1 + norm2) / 2.0);
	vec3 forward = normalize(cross(right, up));

	mat3 rotMat = mat3(
	right, up, forward
	);
	// right, up, forward (original)
	
	float val = 0;
	
	if (toroid)
	{
		float alpha = 1.0 / thickness;		
		vec3 newscale = scale / (alpha + 1.0);
		
		poss =  inverse(rotMat) *( poss) / (newscale * 0.5);

		float tval = pow((pow(abs(poss.z), 2.0/theta) + pow(abs(poss.x), 2.0/theta)), theta/2.0);
		val  = pow(abs(tval - alpha), 2.0/phi) + pow(abs(poss.y), 2.0/phi) - 1.0;
	}
	else
	{
		poss =  inverse(rotMat) *( poss) / (scale * 0.5);
		val = pow((pow(abs(poss.z), 2.0/theta) + pow(abs(poss.x), 2.0/theta)), theta/phi) + pow(abs(poss.y),2.0/phi) - 1.0;
	}
	
	if (selected)	// if selected
	{
		if (val < 0 && elemssize > 0)
		{
			if (previewer)
				discard;
			else
				final_color = final_color * 1.5;
		}
	}
}

// ************-------- Main function --------***************//
void main()
{
	vec3 newN = n;
	
	if (!gl_FrontFacing)
		newN = -newN;
		
	int newShininess;
	newShininess = shininess;
	
	if (iselem)
		newShininess = 128;
	
	if (shadingnum == 0)
		phongLighting(newN, newShininess);
	else
		toon(newN);

	// convert mouse world coords to view coords (so same as v)
	vec4 mouseV = gl_ModelViewMatrix * vec4(vec3(mouse), 1);
	//vec4 mouseV = vec4(vec3(mouse), 1);
	
	superquad();
		/*float minDist = brushSize;		
		if (previewer)
		{
			float d = 1000;
			if (d < minDist)
			{			
				//discard;
				//final_color = final_color * vec4(0.5, 0.5, 0.5, 1);
			}
			else if (d < 1.75 * minDist)
			{
				//float dd = d / (1.5 * minDist);
				//final_color = vec4(1, 0.7, 0.4,  dd - 0.55);
			}
		}*/		
	gl_FragData[0] = final_color;
		
	if (outline == true)
		gl_FragData[0] = vec4(1.0, 0.5, 0, gl_Color.a);
	
	// Encode normals to second texture (sourceNormal)
	vec3 encodedN = newN * 0.5 + 0.5;
	gl_FragData[1] = vec4(encodedN, 1);
	
	// [2] is Capping Mask (Sent in Phong Shader)
}

