/*******************************************************************
Fragment
Pre-pass Shader : Pass fragment data to main shader
*******************************************************************/

#version 440 compatibility

// Input from vs
smooth in vec3 n;
smooth in vec3 v;
smooth in vec3 original_v;

// Output fragments
layout(location = 0) out vec4 o_depthSelectedF;
layout(location = 1) out vec4 o_depthSelected;
layout(location = 2) out vec4 o_depthSQ;

// Uniforms
uniform vec2 frameBufSize;

uniform bool selected = false;
uniform bool iselem = false;

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

//------------------------------------------------
vec3 PackFloat8bitRGB(float val) {
    vec3 pack = vec3(1.0, 255.0, 65025.0) * val;
    pack = fract(pack);
    pack -= vec3(pack.yz / 255.0, 0.0);
    return pack;
}
//------------ Linearize Depth ------------------------
float LinearizeDepth(float depth)
{
  float n = 0.1; // camera z near
  float f = 1000.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}
//------------ Draw Superquad (discard) -----------------
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
			discard;
		}
	}
}

// ****************  Main function *************************
void main()
{
	vec3 newN = n;
	
	if (!gl_FrontFacing)
		newN = -newN;

	//float d = gl_FragCoord.z;
	vec3 d = PackFloat8bitRGB(gl_FragCoord.z);

	superquad();
		
	if (selected)
	{
		o_depthSelectedF = vec4(1, 1, 1, 1);

		if (gl_FrontFacing)
		{			
			o_depthSelectedF = vec4(d, 1);
		}
		else
			o_depthSelected = vec4(d, 1);
	}
	
	if (iselem)
	{
		if (gl_FrontFacing)
			discard;
		else
			o_depthSQ = vec4(d, 1);
	}	
}

