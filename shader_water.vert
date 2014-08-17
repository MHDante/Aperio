/*******************************************************************
	Vertex
	Water Shader : Rippling water shader
*******************************************************************/

#version 440 compatibility
//#version 130
//#extension GL_EXT_geometry_shader4 : enable

uniform float time = 0.0;
uniform bool selected;
uniform bool wiggle;

out vec3 n;
out vec3 v;
out vec3 original_v;

// Values
const float pi = 3.14159;
float waterHeight = 0.0f;
int numWaves = 8;

float amplitude[8];
float wavelength[8];
float speed[8];

vec2 direction[8];

vec4 final_position;

float wave(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + time * phase);
}

float waveHeight(float x, float y) {
	
    float height = 0.0;
    for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, y);
    return height;
}

float dWavedx(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < numWaves; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, -dy, 1.0);
    return normalize(n);
}

void water()
{
   float a = 0.01;
   amplitude = float[8](a, a, a, a, a, a, a, a);

   float w = 0.04;
   wavelength = float[8](w, w, w, w, w, w, w, w);

   float s = 0.1;
   speed = float[8](s, s, s, s, s, s, s, s);

   vec2 dd = vec2(0.0, 0.01);   
   direction = vec2[8](dd, dd, dd, dd, dd, dd, dd, dd);

	vec4 pos = gl_Vertex;
    pos.z = pos.z + (waterHeight + waveHeight(pos.x, pos.y)) * 0.25;
	
	final_position = gl_ModelViewProjectionMatrix * (pos);
}

void main()  
{     
   	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
    n = normalize(gl_NormalMatrix * gl_Normal);

	original_v = vec3(gl_Vertex);
	
	gl_FrontColor = gl_Color;
	gl_BackColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;   

	final_position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	if (selected == true && wiggle == true)
		water();

	gl_Position = final_position;
}
