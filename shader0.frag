/*******************************************************************
Fragment
Pre-pass Shader : Run on fragment data
*******************************************************************/

#version 450 compatibility

smooth in vec3 n;
smooth in vec3 v;
smooth in vec3 original_v;

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

float LinearizeDepth(float depth)
{
  float n = 1; // camera z near
  float f =750.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}

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

// ************-------- Main function --------***************//
void main()
{
	vec3 newN = n;
	
	if (!gl_FrontFacing)
		newN = -newN;
	
	superquad();

	//gl_FragData[0] = vec4(1, 1, 1, 1);
	//gl_FragData[1] = vec4(1, 1, 1, 1);
	//gl_FragData[2] = vec4(1, 1, 1, 1);

	//float o = gl_FragCoord.z;
	float o = LinearizeDepth(gl_FragCoord.z);
	
	if (selected)
	{
		if (gl_FrontFacing)
			gl_FragData[0] = vec4(o, o, o, 1);
		else
			gl_FragData[1] = vec4(o, o, o, 1);
		//gl_FragData[1] = vec4(newN, 1);
	}
	
	if (iselem)
	{
		//Backfacing camera
		//if (dot(newN, vec3(0, 0, 1)) <= 0)				
		if (gl_FrontFacing)
			discard;
		else
			gl_FragData[2] = vec4(o, o, o, 1);
	}	
}

