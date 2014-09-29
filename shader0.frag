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

float LinearizeDepth(float depth)
{
  float n = 0.01; // camera z near
  float f = 5000.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}

// ************-------- Main function --------***************//
void main()
{
	vec3 newN = n;
	
	if (!gl_FrontFacing)
		newN = -newN;
	
	//gl_FragData[0] = vec4(1, 1, 1, 1);
	//gl_FragData[1] = vec4(1, 1, 1, 1);
	//gl_FragData[2] = vec4(1, 1, 1, 1);

	float o = gl_FragCoord.z;
	//float o = LinearizeDepth(gl_FragCoord.z);
	
	
	if (selected)
	{
		if (gl_FrontFacing)	
			gl_FragData[0] = vec4(o, o, o, 1);
		else
			gl_FragData[1] = vec4(o, o, o, 1);
	}
	
	if (iselem)
	{
		if (gl_FrontFacing)
			discard;
		else
			gl_FragData[2] = vec4(o, o, o, 1);
	}
}

