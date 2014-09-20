/*******************************************************************
Fragment
Main Shader : Phong-Minneart Reflection model and main interaction
*******************************************************************/

#version 440 compatibility

smooth in vec3 n;
smooth in vec3 v;
smooth in vec3 original_v;

// Shader uniforms
uniform sampler2D source;
uniform sampler2D sourceBump;

uniform bool outline = false;
uniform bool iselem = false;

uniform bool selected = false;
uniform bool peerInside = false;
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

uniform mat4 transMat;
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
void phongLighting(vec3 n)
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
	vec3 tex = texture2D(source, gl_TexCoord[0].st * 20).rgb;		
	
	final_color = vec4(myspecular +  int(difftrans) * diffuseTranslucency + 1.0*Idiff.xyz + 0.0*tex + Iamb.xyz, gl_Color.a);
	
	if (iselem)
	{
		vec3 tex = texture2D(source, gl_TexCoord[0].st * 5).rgb;
		
		final_color = vec4(		
		1*myspecular +  0.8* int(difftrans) * diffuseTranslucency + 1.0 * (
		(Idiff.rgb + vec3(0.35,0.35,0.35)) * tex ) - 0.0 * Iamb.xyz
		//, 0.5) ;
		, gl_Color.a) ;
		//0.25 alpha in-program
	}
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
void subScatterFS(vec3 n)
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

// ************-------- Main function --------***************//
void main()
{
	vec3 newN = n;
	
	if (!gl_FrontFacing)
		newN = -newN;
		
	if (shadingnum == 0)
		phongLighting(newN);
	else if (shadingnum == 1)
		subScatterFS(newN);
	else
		toon(newN);

	// convert mouse world coords to view coords (so same as v)
	vec4 mouseV = gl_ModelViewMatrix * vec4(vec3(mouse), 1);
	//vec4 mouseV = vec4(vec3(mouse), 1);
	
	float ee = myexp;
	float nn = 0.5;

	//float d = pow(pow(abs(original_v.y - pos1.y) / 1.0, 2.0 / ee) + pow(abs(original_v.x - pos1.x) / 1.0, 2.0 / ee), ee / nn) + pow(abs(original_v.z - pos1.z) / 1.0, 2.0 / nn) - 1.0;
	
	
	//vec3 pp = vec3( (xyz[0] - this->Center[0]) / s[0], 
	//(xyz[1] - this->Center[1]) / s[1],
	//(xyz[2] - this->Center[2]) / s[2]
	//);
	
	float dist = distance(pos1, pos2);
	vec3 poss = original_v - (pos1 + pos2) / 2.0;
		
	// Rotation matrix
	vec3 right = normalize(pos2 - pos1);	
	vec3 forward = normalize((norm1 + norm2) / 2.0);
	vec3 up = normalize(cross(forward, right));
	
	mat3 rotMat = mat3(
	right, up, forward
	);
	
	/*mat4 translateMat = mat4(
	vec4(1, 0, 0, ),
	vec4(0, 1, 0, ),
	vec4(0, 0, 1, ),
	vec4(0, 0, 0, 1)
	);
	translateMat = transpose(translateMat);*/
	 //poss = poss / scale;
	//poss =  rotMat * ( poss) / scale;
	poss =  inverse(rotMat) *( poss) / (scale * 0.5);
	
    float val = pow((pow(abs(poss.z), 2.0/theta) + pow(abs(poss.x), 2.0/theta)), theta/phi) + pow(abs(poss.y),2.0/phi) - 1.0;
	 
	  

//	float d = pow(pow(abs(original_v.y - mouse.y) / 1.0, 2.0 / ee) + pow(abs(original_v.x - mouse.x) / 1.0, 2.0 / ee), ee / nn) + pow(abs(original_v.z - mouse.z) / 1.0, 2.0 / nn) - 1.0;
	//float d = pow(pow(abs(v.y - mouseV.y) / 1.0, 2.0 / ee) + pow(abs(v.x - mouseV.x) / 1.0, 2.0 / ee), ee / nn) + pow(abs(v.z - mouseV.z) / 1.0, 2.0 / nn) - 1.0;
	//float d = pow(pow(abs(original_v.y - mouseV.y) / 1.0, 2.0/ee) + pow(abs(original_v.x - mouseV.x) / 1.0, 2.0/ee), ee/nn) + pow(abs(original_v.z - mouseV.z) / 1.0, 2.0/nn) - 1.0;
	
	if (!gl_FrontFacing && iselem)
	{
		//final_color.a = 1;
		//final_color = vec4(1, 0, 0, 1);
		//final_color = gl_Color;
	}
	if (gl_FrontFacing && iselem)
	//if (gl_FrontFacing)
	{
		//discard;
		//final_color.a = 1;
		//final_color = vec4(1, 1, 1, 0.1);
	}
	
	if (selected)	// if selected
	{
	
		/*if(val > MAX_FVAL){
		//val = MAX_FVAL;
		discard;
		}
		else if(val < -MAX_FVAL){
		//val = -MAX_FVAL;
			final_color = vec4(1, 1, 1, 1);
		}*/
		
		/*if (val > 100000000)
			val = 100000000;
		if (val < -100000000)
			val = -100000000;*/
			
		if (val < 0 && elemssize > 0)
			discard;
			//final_color = vec4(final_color.rgb * 1.75, final_color.a);
			
		else
			;//final_color = vec4(1, 1, 1, 1);
		/*float fraction = fract(time);
		float multiplier = 0;
		float result = 0;
		
		if (fraction < 0.5)
			multiplier = (fraction / 0.5);
		else
			multiplier = (1.0 - fraction) / 0.5;
		
		result = multiplier * 0.25 + 0.85;
		final_color = vec4(final_color.rgb * result, final_color.a);*/
		
		float minDist = brushSize;
		
		if (peerInside)
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
		}
	}
	gl_FragData[0] = final_color;
	
	if (outline == true)
		gl_FragData[0] = vec4(1.0, 0.5, 0, gl_Color.a);
	
	// Encode normals to second texture (sourceNormal)
	vec3 encodedN = newN * 0.5 + 0.5;
	gl_FragData[1] = vec4(encodedN, 1);

	// Encode noise? for third texture (sourceNoise) TBA
	gl_FragData[2] = vec4(0.5, 1, 1, 1);
}

