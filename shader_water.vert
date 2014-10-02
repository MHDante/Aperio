/*******************************************************************
	Vertex
	Water Shader : Rippling water shader
*******************************************************************/

#version 440 compatibility

// Output to fs
out vec3 n;
out vec3 v;
out vec3 original_v;

out vec4 vTexCoord;

//--- Uniforms
uniform float time = 0.0;
uniform bool selected = false;
uniform bool wiggle = false;

//--- Wave Parameters
const float pi = 3.141592653;
float waterHeight = 0.0f;
const int numWaves = 8;

// Set up constant arrays
const float a = 0.01f;
const float w = 0.04f;
const float s = 0.1f;
const vec2 d = vec2(0.0, 0.01);

const float amplitude[] = float[](a, a, a, a, a, a, a, a);
const float wavelength[] = float[](w, w, w, w, w, w, w, w);
const float speed[] = float[](s, s, s, s, s, s, s, s);
const vec2 direction[] = vec2[](d, d, d, d, d, d, d, d);

vec4 final_position;

//------------------------ Wave Function -----------------------
float wave(const in int i, const in float x, const in float y) 
{
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + time * phase);
}
//------------------------ Wave Height -----------------------
float waveHeight(const in float x, const in float y) 
{
    float height = 0.0;
    for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, y);
    return height;
}
//------------------------ Water function ---------------------
void water()
{
	vec4 pos = gl_Vertex;
    pos.z = pos.z + (waterHeight + waveHeight(pos.x, pos.y)) * 0.25;
	
	final_position = gl_ModelViewProjectionMatrix * (pos);
}
//---------- Interesting Barrel Distort/Deform ----------------
vec4 Distort(vec4 p)
{
    vec2 v = p.xy / p.w;

    // Convert to polar coords:
    float theta  = atan(v.y,v.x);
    float radius = length(v);

    // Distort:
	float Power = 1.0f;
    radius = pow(radius, Power);

    // Convert back to Cartesian:
    v.x = radius * cos(theta);
    v.y = radius * sin(theta);
    p.xy = v.xy * p.w;
    return p;
}

//********************* Main ************************
void main()  
{     
   	v = vec3(gl_ModelViewMatrix * gl_Vertex);
    n = normalize(gl_NormalMatrix * gl_Normal);
	original_v = gl_Vertex.xyz;
	
	gl_FrontColor = gl_Color;
	gl_BackColor = gl_Color;
	vTexCoord = gl_MultiTexCoord0;   

	final_position = gl_ModelViewProjectionMatrix * (gl_Vertex);
	
	if (selected == true && wiggle == true)
		water();
	
	gl_Position = final_position ;
}
