/*******************************************************************
Fragment
Main Shader : Phong-Minneart Reflection model and main interaction
*******************************************************************/

#version 420 compatibility

// Input from vs
in vec3 n;
in vec3 v;
in vec3 original_v;

in vec4 vTexCoord;

// Outputs
layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oDepth;
layout(location = 3) out vec4 oCapMask;

//--- Uniforms
uniform vec2 frameBufSize = vec2(800, 600);

//--- Shader variables
uniform sampler2D depthSelectedF;
uniform sampler2D depthSelected;
uniform sampler2D depthSQ;
uniform sampler2D selectedColours;
uniform sampler2D matcap;

uniform bool toolTipOn = false;
uniform bool cap = true;

uniform bool active_elem = false;

uniform bool outline = false;
uniform bool iselem = false;

uniform bool selected = false;
uniform bool previewer = true;
uniform bool difftrans = true;

uniform vec3 selectedColor = vec3(0.5, 0.5, 0.5);

uniform vec3 mouse = vec3(0, 0, 0);
uniform float mouseSize = 1.0;
uniform float brushSize = 1.5;

uniform int shadingnum = 0;
uniform int shininess = 128;
uniform float darkness = 1.0;

uniform float time = 0.0;

//--- Superquad data
uniform vec3 pos1 = vec3(0, 0, 0);
uniform vec3 pos2 = vec3(0, 0, 0);
uniform vec3 norm1 = vec3(0, 0, 0);
uniform vec3 norm2 = vec3(0, 0, 0);
uniform vec3 scale = vec3(0, 0, 0);

uniform vec3 right = vec3(0, 0, 0);
uniform vec3 up      = vec3(0, 0, 0);
uniform vec3 forward = vec3(0, 0, 0);
uniform float angle = 0.0;
uniform float tilt = 0.0;

uniform float phi = 0.0;
uniform float theta = 0.0;
uniform float thickness = 0.4;
uniform float taper = 0.0;

uniform bool toroid = false;
uniform bool cutter = true;

uniform bool  ribbons         = false;
uniform bool  frontRibbons    = true;
uniform float ribbonWidth     = 0.2;
uniform int   ribbonFrequency = 5;
uniform float   ribbonTilt 	  = 0.1;


uniform int elemssize;

//--- Constant Light parameters
const vec4 light_ambient = vec4(0.2, 0.2, 0.2, 1);
const vec4 light_diffuse = vec4(0.6, 0.6, 0.6, 1);
const vec4 light_specular = vec4(1.0, 1.0, 1.0, 1);
const vec3 light_position = normalize(vec3(-0.080999853,6.4752009809,2.6762204566));

vec3 E = normalize(-v); // in Eye coords, so EyePos is (0,0,0) surf2Eye  	
vec3 L = light_position;
vec3 R = normalize(-reflect(L, n));  // Reflection of surf2Light and normal
vec3 h = normalize(E + L);

// Global variables and Constants
vec4 final_color = vec4(1, 1, 1, 1);

const float PI = 3.141592653;

vec3 newN;

//---------------- Minnaert limb darkening diffuse term --------------
vec3 minnaert(vec3 L, vec3 n, float k, vec3 light_color) 
{
	float ndotl = max(0.0, dot(L, n));
	return  light_color * pow(ndotl, k);
}
//------------------------------------------------
float linearizeDepth(const in float depth) 
{
	float zNear = 1.0f;
	float zFar = 100.0f;
	//float zNear = 0.1f;
	//float zFar = 1000.0f;
	return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}
//------------------------------------------------
vec3 PackFloat8bitRGB(float val) {
    vec3 pack = vec3(1.0, 255.0, 65025.0) * val;
    pack = fract(pack);
    pack -= vec3(pack.yz / 255.0, 0.0);
    return pack;
}
//------------------------------------------------
float UnpackFloat8bitRGB(vec3 pack) {
    return dot(pack, vec3(1.0, 1.0 / 255.0, 1.0 / 65025.0));
}
//------------------------------------------------
float getDepth(sampler2D sampler, vec2 uv)
{
	return linearizeDepth(UnpackFloat8bitRGB(texture(sampler, uv).rgb));
}
//---------------- Phong lighting (Directional) ----------------------
void phongLighting(vec3 n, int shininess)
{
	//--- calculate Ambient Term:    
	vec4 theamb = gl_FrontMaterial.ambient;
	theamb.b /= 1.4;
	vec4 Iamb = (theamb * light_ambient * 1.25);
	
	//--- Computation of forward scattered translucency: 
	vec3 _DiffuseTranslucentColor = vec3(1,1,1);

	float att = 1.0;
	vec3 diffuseTranslucency = att * light_specular.rgb
		* vec3(_DiffuseTranslucentColor)
		* max(0.0, dot(L, -n));

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

	// Special Idiff2 for Superquad's backface
	vec4 Idiff2 = vec4(0, 0, 0, 0);		
	Idiff2 += vec4(minnaert(L, n, roughness, light_color), 0);
	Idiff2 = vec4(Idiff2.rgb * selectedColor.rgb, 1.0);	
	Idiff2 += vec4(minnaert(L2, n, roughness, light_color2), 0);	
	Idiff2 = clamp(Idiff2, 0.0, 1.0);
	
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

	//myspecular = clamp(myspecular, 0.0, 0.999999);
	myspecular = clamp(myspecular, 0.0, 1.0);

	//---- Final color	
	final_color = vec4(myspecular +  int(difftrans) * diffuseTranslucency + 1.0*Idiff.xyz + Iamb.xyz, gl_Color.a);
	
	
	//--- Superquad color
	if (iselem && !cutter)
	{
		vec3 tex = vec3(1, 1, 1);
		
		// different opacities for front and back faces
		float op = 0;
		if (gl_FrontFacing)
			op = 1.0;
		else
			op = 0.2;
		
		final_color = vec4(		
		1*myspecular +  0.8* int(difftrans) * diffuseTranslucency + 1.0 * (
		(Idiff.rgb + vec3(0.35,0.35,0.35)) * tex ) - 0.0 * Iamb.xyz
		, op) ;
	}
		
	//--- Capping algorithm
	if (cap && iselem && !gl_FrontFacing)
	{
		vec2 texpos = vec2(gl_FragCoord.x / frameBufSize.x, gl_FragCoord.y / frameBufSize.y);	
		
		vec4 v_selectedF = texture2D(depthSelectedF, texpos);
		vec4 v_selected = texture2D(depthSelected, texpos);
		vec4 v_SQ = texture2D(depthSQ, texpos);
		
		float d_selectedF = getDepth(depthSelectedF, texpos);
		float d_selected = getDepth(depthSelected, texpos);
		float d_SQ = getDepth(depthSQ, texpos);
				
		if (
		(
		v_selectedF.r >= 1.0 && 
		d_SQ < d_selected)
		)
		{
			Idiff = vec4(0, 0, 0, 0);	
			
			vec4 wallcolour = texture(selectedColours, texpos);
			vec3 light_color = vec3(0.8,0.8,0.8);
			Idiff += vec4(minnaert(L, n, roughness, light_color), 0);
			Idiff = vec4(Idiff.rgb * wallcolour.rgb, 1.0);	
			Idiff = clamp(Idiff, 0.0, 1.0);
		
			final_color = vec4(		
		1.0*myspecular +  0.8* int(difftrans) * diffuseTranslucency + 1.0 * (Idiff.rgb) + 0.2*wallcolour.rgb - 0.0 * Iamb.xyz, 1.0);
			
			// Capping Mask for SSAO shader
			oCapMask = vec4(1, 1, 1, 1);

		}
		else
			discard;
	}	
	
	if (iselem && gl_FrontFacing && cutter)
	{
		discard;
		//superquadelem();
	}
	
	if (iselem && gl_FrontFacing == false)
	{
	//discard;
	}

	
	/*if (cap && selected && gl_FrontFacing)
	{
		vec2 texpos = vec2(gl_FragCoord.x / frameBufSize.x, gl_FragCoord.y / frameBufSize.y);	
		
		vec4 v_selectedF = texture2D(depthSelectedF, texpos);
		vec4 v_selected = texture2D(depthSelected, texpos);
		vec4 v_SQ = texture2D(depthSQ, texpos);
		
		float d_selectedF = getDepth(depthSelectedF, texpos);
		float d_selected = getDepth(depthSelected, texpos);
		float d_SQ = getDepth(depthSQ, texpos);
		
		if (d_SQ < d_selectedF)
		{
			final_color = vec4(1, 1, 0, 1);
			
		
		}
	}*/
			
	//--- Test display of textures read from Pre-pass
	vec2 texpos = vec2(gl_FragCoord.x / frameBufSize.x, gl_FragCoord.y / frameBufSize.y);	
	
	vec4 v_selectedF = texture2D(depthSelectedF, texpos);
	vec4 v_selected = texture2D(depthSelected, texpos);
	vec4 v_SQ = texture2D(depthSQ, texpos);
	
	float d_selectedF = getDepth(depthSelectedF, texpos);
	float d_selected = getDepth(depthSelected, texpos);
	float d_SQ = getDepth(depthSQ, texpos);

	vec4 v_selectedColours = texture2D(selectedColours, texpos);

	//final_color = vec4(vec3(d_selectedF), 1);
	
	//final_color = vec4(vec3(v_selectedColours), 1);
	
	//final_color = texture(selectedColours, texpos);
	
	if (d_SQ > d_selectedF)
	{
		//final_color = vec4(1, 1, 0, 1);
	}
		
	
	
	//final_color = vec4(1, 1, 1, 1);
	
}

//---------------- Toon Shader -----------------------------------
void toon(vec3 n)
{
	const int levels = 5;
	const float scaleFactor = 1.0 / levels;

	vec4 col = gl_Color;

	float intensity = max(dot(n, normalize(L)), 0.0);
	vec3 diffuse = gl_Color.rgb * floor(intensity * levels) * scaleFactor;
	final_color = gl_Color / 20.0f + vec4(diffuse, gl_Color.a);
}

mat3 rotationMatrix(float angle, vec3 axis)
{
  axis = normalize(axis);
  float s = sin(angle);
  float c = cos(angle);
  float oc = 1.0 - c;
  return mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 
              oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 
              oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c); 
}

//---------------- Draw Superquad (Discard) ------------------------
void superquad()
{
	float dist = distance(pos1, pos2);
	vec3  poss = original_v - (pos1 + pos2) / 2.0;
	
	mat3  rotMat = mat3(right, up, forward);
	mat3  spinMat = rotationMatrix(-angle,up);
	mat3  pitchMat = rotationMatrix(-tilt,right);

	vec3 forwardSpin = spinMat * forward;
	mat3 tiltMat = rotationMatrix(ribbonTilt,forwardSpin);
	vec3 rightspin1 = spinMat * right;
	vec3 rightspin = tiltMat * rightspin1;
			
    vec3 cp = (pos1+pos2)/2.0;
	
	//mat3 rotMat = rotMati * spinMat;

	float val = 0;
	
	if (cutter)
	{
		poss = inverse(spinMat) * poss;
		poss = inverse(rotMat) * poss;
		poss = poss / (scale * 0.5);
	
	    //poss =  inverse(rotMat) *( poss) / (scale * 0.5);
		val = pow((pow(abs(poss.z), 2.0/theta) + pow(abs(poss.x), 2.0/theta)), theta/phi) + pow(abs(poss.y),2.0/phi) - 1.0;
		
		if (selected)	// if selected
		{
			//final_color = vec4(1, 1, 1, 1);

			//final_color = vec4(1, 1, 1, 1);
			if (val < 0 && elemssize > 0)
			{			
			    if (ribbons)
				{
					if (fract(dot((original_v-cp), rightspin)*ribbonFrequency) - ribbonWidth < 0)
					{
						if (!gl_FrontFacing)
						{
							if (frontRibbons)
								final_color.rgb = final_color.rgb;
							else
								discard;
						}
						else
							final_color.rgb = final_color.rgb * 1.25;
					}
					else
						discard;
				}
				else if (previewer)
					discard;
				else
					final_color = final_color * 1.5;
			}
		}
	}
}
void RimLight()
{
	//vec4 RimColor = vec4(0.26,0.19,0.16,0.0);
	//vec4 RimColor = vec4(1.0,1.0,1.0,1.0);
	vec4 RimColor = vec4(0.5,0.5,0.5,1.0);
	float RimPower = 3.0;
	float rim = 1.0 - clamp(dot (normalize(E), newN), 0.0, 1.0);
	oColor.rgb = oColor.rgb + RimColor.rgb * pow(rim, RimPower);
}

void Highlight()
{
	if (selected)	
	{
		if (fract((original_v.y + original_v.z * 0.01) * 5) - 0.9 < 0)		
			oColor.rgb = oColor.rgb * 1.05;
			//discard;
		else
			oColor.rgb = oColor.rgb * 1.25;
			//discard;
			
		//if (fract((original_v.x + original_v.z * 0.01) * 5) - 0.9 < 0)		
		//	oColor.rgb = oColor.rgb * 1.05;
		//else
		//	oColor.rgb = oColor.rgb * 1.25;
	}		
}

void SEM()
{
// SEM (matcap)
	vec3 r = normalize(reflect(v, newN));	
	float m = 2. * sqrt(pow(r.x, 2.) + pow(r.y, 2.) + pow(r.z + 1., 2.) );	
	
	vec2 vN = r.xy / m + .5;
	vec3 bases = texture(matcap, vN).rgb;	
	//vec3 final = mix(bases, final_color.rgb, 0.85);
	vec3 final = mix(bases, final_color.rgb, 0.0);
	
	oColor = vec4(final, 1.0);
}

//******************* Main **************************************
void main()
{
	newN = n;
	
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
	
	superquad();

	oColor = final_color;		
	
	if (
	(iselem && !cutter) || outline
	)
	{
		SEM();
	}
	
	vec2 texpos = vec2(gl_FragCoord.x / frameBufSize.x, gl_FragCoord.y / frameBufSize.y);	
	
	vec4 wallcolour = texture(selectedColours, texpos);
	
	// if it is an outline for the "Knife" (check knife)
	if (outline && wallcolour.a <= 0)
		;//discard;
	//{
	//}

	if (outline)
	;//discard;
	
	Highlight();	
	RimLight();
	
	// Outline
	/*if (outline == true)
	{
		//oCapMask = vec4(1, 1, 1, 1);
		//oColor = vec4(1.0, 1.0, 1.0, 1.0);
		//oColor = vec4(vec3(0.9), 1.0);
	}*/
	
	//if (outline)
	//discard;
	
	// Encode normals to second texture (sourceNormal)
	vec3 encodedN = newN * 0.5 + 0.5;
	oNormal = vec4(encodedN, 1);
	
	oDepth = vec4(PackFloat8bitRGB(gl_FragCoord.z), 1);
}
