/*******************************************************************
Fragment
Main Shader : Phong-Minneart Reflection model and main interaction
*******************************************************************/

#version 440 compatibility
//#version 130
//#extension GL_EXT_geometry_shader4 : enable

in vec3 n;
in vec3 v;
in vec3 original_v;

uniform vec3 mouse = vec3(0, 0, 0);
uniform float mouseSize = 1.0;
uniform float brushSize = 1.5;
uniform bool peerInside = false;

uniform float myexp = 1.0;
uniform sampler2D source ;

uniform int shadingnum = 0;
uniform bool difftrans = true;
uniform int shininess = 128;

uniform float darkness = 1.0;

uniform bool selected = false;
uniform bool iselem = false;

// Light parameters
vec4 light_ambient = vec4(0.2, 0.2, 0.2, 1);
vec4 light_diffuse = vec4(0.6, 0.6, 0.6, 1);
vec4 light_specular = vec4(1.0, 1.0, 1.0, 1);
vec3 light_position = normalize(vec3(-0.080999853,6.4752009809,2.6762204566));

// Global variables/Constants
vec4 final_color = vec4(1, 1, 1, 1);

const float PI = 3.141592653;

//---Minnaert limb darkening diffuse term
vec3 minnaert(vec3 L, vec3 n, float k, vec3 light_color) {
	float ndotl = max(0.0, dot(L, n));
	return  light_color * pow(ndotl, k);
}

//---------------- Phong lighting (Directional) ----------------------//
void phongLighting(int i)
{
	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0) surf2Eye  	
	//L = normalize(vec3(-0.080999853,6.4752009809,3.6762204566)); // surf2Light, for directional lights
	vec3 L = light_position;
	vec3 R = normalize(-reflect(L, n));  // Reflection of surf2Light and normal
	vec3 h = normalize(E + L);

	//calculate Ambient Term:    
	vec4 theamb = gl_FrontMaterial.ambient;
	theamb.b /= 1.4;
	vec4 Iamb = (theamb * light_ambient);

	//--- Computation of forward scattered translucency: 
	vec3 _DiffuseTranslucentColor = vec3(1,1,1);
	vec3 _ForwardTranslucentColor = vec3(1, 1, 1);
	float _Sharpness = 0.5f;

	float att = 0.8;
	vec3 diffuseTranslucency = att * light_specular.rgb
		* vec3(_DiffuseTranslucentColor)
		* max(0.0, dot(L, -n));

	vec3 forwardTranslucency;
	if (dot(n, L) > 0.0) // light source on the wrong side?
	{
		forwardTranslucency = vec3(0.0, 0.0, 0.0); // no forward-scattered translucency
	}
	else // light source on the right side
	{
		forwardTranslucency = att * light_specular.rgb
			* vec3(_ForwardTranslucentColor) * pow(max(0.0, dot(-L, E)), _Sharpness);
	}

	//--- Minnaert for darker diffuse (moon shading)
	float roughness = darkness; // minnaert roughness  1.5 default (1.0 is lambert)
	vec4 Idiff = vec4(0, 0, 0, 0);	

	vec3 light_color = light_diffuse.rgb;
	Idiff += vec4(minnaert(L, n, roughness, light_color), 0);
	Idiff = vec4(Idiff.rgb * gl_FrontMaterial.diffuse.rgb, 1.0);	

	//vec3 light_color2 = vec3(0.325f, 0.035f, 0.0f);
	vec3 light_color2 = vec3(0.325f, 0.035f, 0.0f);
	//vec3 L2 = normalize(vec3(1, 1, 0.25));
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

	int difftransamount = 1;	
	if (difftrans == false)
		difftransamount = 0;
	
	//--- Final color
	final_color = vec4(myspecular +  difftransamount * diffuseTranslucency + Idiff.xyz + Iamb.xyz, gl_Color.a);
	
	if (iselem == true)
	{
		vec3 tex = texture2D(source, gl_TexCoord[0].st * 5).rgb;
		
		final_color = vec4(		
		1*myspecular +  0.8* difftransamount * diffuseTranslucency + 1.0 * ( (Idiff.rgb + vec3(0.35,0.35,0.35)) * (tex - vec3(.0,.0,.0) ) ) - 0.0 * Iamb.xyz
		, 0.5) ;
	}
}

// ---------------- Toon Shader ----------------------//
void toon()
{
	const int levels = 5;
	const float scaleFactor = 1.0 / levels;

	vec4 col = gl_Color;

	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0) surf2Eye  
	vec3 L = normalize(light_position);   // surf2Light
	vec3 R = normalize(-reflect(L, n));  // Reflection of surf2Light and normal

	float intensity = max(dot(n, normalize(L)), 0.0);
	vec3 diffuse = gl_FrontMaterial.diffuse.rgb * floor(intensity * levels) * scaleFactor;
	final_color = gl_FrontMaterial.ambient / 20.0f + vec4(diffuse, gl_Color.a);
}

// ---------------- Subsurface Scatter Shader (Approximate) ----------------------//
float halfLambert(in vec3 vect1, in vec3 vect2)
{
	float product = dot(vect1, vect2);
	return product * 0.5 + 0.5;
}
float blinnPhongSpecular(in vec3 normalVec, in vec3 lightVec, in float specPower)
{
	vec3 halfAngle = normalize(normalVec + lightVec);
	return pow(clamp(0.0, 1.0, dot(normalVec, halfAngle)), specPower);
}
// Main fake sub-surface scatter lighting function
void subScatterFS()
{
	// Variables for lighting properties
	float MaterialThickness = 0.6f;
	vec3 ExtinctionCoefficient = vec3(0.8f, 0.2f, 0.12f); // Will show as X Y and Z ports in QC, but actually represent RGB values.
	vec4 LightColor = vec4(.2, .2, .2, 1);
	vec4 BaseColor = gl_Color * 7.0f;//vec4(0.5f, 0.5f, 0.5f, 1);
	vec4 SpecColor = vec4(-0.4, -0.4, -0.4, 1);
	float SpecPower = 0.0f;
	float RimScalar = 10.0f;
	//uniform sampler2D Texture;

	// Varying variables to be sent to Fragment Shader
	vec3 eyeVec = -v;
	vec3 lightPos = light_position;
	vec3 lightVec = lightPos - v.xyz;

	float attenuation = 0.8f;//10.0 * (1.0 / distance(lightPos,v));
	vec3 eVec = normalize(eyeVec);
	vec3 lVec = normalize(lightVec);
	vec3 wNorm = normalize(n);

	vec4 dotLN = vec4(halfLambert(lVec, wNorm) * attenuation);
	//dotLN *= texture2D(Texture, gl_TexCoord[0].xy);
	dotLN *= BaseColor;

	vec3 indirectLightComponent = vec3(MaterialThickness * max(0.0, dot(-wNorm, lVec)));
	indirectLightComponent += MaterialThickness * halfLambert(-eVec, lVec);
	indirectLightComponent *= attenuation;
	indirectLightComponent.r *= ExtinctionCoefficient.r;
	indirectLightComponent.g *= ExtinctionCoefficient.g;
	indirectLightComponent.b *= ExtinctionCoefficient.b;

	vec3 rim = vec3(1.0 - max(0.0, dot(wNorm, eVec)));
	rim *= rim;
	rim *= max(0.0, dot(wNorm, lVec)) * SpecColor.rgb;

	final_color = dotLN + vec4(indirectLightComponent, 1.0);
	final_color.rgb += (rim * RimScalar * attenuation * final_color.a);
	final_color.rgb += vec3(blinnPhongSpecular(wNorm, lVec, SpecPower) * attenuation * SpecColor * final_color.a * 0.05);
	final_color.rgb *= LightColor.rgb;

	final_color.a = gl_Color.a;
}

// ***************-------------------- Main function -------------------------***************//
void main()
{
	vec3 newN = n;
	
	if (!gl_FrontFacing)
		newN = -newN;
		
	if (shadingnum == 0)
		phongLighting(0);
	else if (shadingnum == 1)
		subScatterFS();
	else
		toon();

	// convert mouse world coords to view coords (so same as v)
	vec4 mouseV = gl_ModelViewMatrix * vec4(vec3(mouse), 1);
	//vec4 mouseV = vec4(vec3(mouse), 1);

	float ee = myexp;
	float nn = 0.5;

	float d = pow(pow(abs(v.y - mouseV.y) / 1.0, 2.0 / ee) + pow(abs(v.x - mouseV.x) / 1.0, 2.0 / ee), ee / nn) + pow(abs(v.z - mouseV.z) / 1.0, 2.0 / nn) - 1.0;
	//float d = pow(pow(abs(original_v.y - mouseV.y) / 1.0, 2.0/ee) + pow(abs(original_v.x - mouseV.x) / 1.0, 2.0/ee), ee/nn) + pow(abs(original_v.z - mouseV.z) / 1.0, 2.0/nn) - 1.0;
	
	if (selected == true)	// if selected
	{
		float minDist = brushSize;
		
		//final_color = vec4(1, 0, 0, 1);
		if (peerInside == true)
		{
		
			if (d < minDist)
			{			
				discard;
				final_color = final_color * vec4(1.75, 1.25, 0.25, 0.35);
				//final_color = final_color * vec4(1.75, 1.25, 0.25, 1 - (0.1 - d) );
				//final_color = final_color * vec4(1.75, 1.25, 0.25, 0.75 - (minDist - d));
				//final_color = final_color * vec4(1.75, 1.25, 0.25, 0.0);
			}
			else if (d < 1.75 * minDist)
			{
				float dd = d / (1.5 * minDist);
				final_color = vec4(1, 0.7, 0.4,  dd - 0.55);
			}

		}
	}	
	gl_FragColor = final_color;	
}

