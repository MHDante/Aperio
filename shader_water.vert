/*******************************************************************
	Vertex
	Water Shader : Rippling water shader
*******************************************************************/

#version 330 compatibility
//#version 130
//#extension GL_EXT_geometry_shader4 : enable

uniform float time;
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
   amplitude[0] = a;
   amplitude[1] = a;
   amplitude[2] = a;
   amplitude[3] = a;
   amplitude[4] = a;
   amplitude[5] = a;
   amplitude[6] = a;
   amplitude[7] = a;

   float w = 0.04;
   wavelength[0] = w;
   wavelength[1] = w;
   wavelength[2] = w;
   wavelength[3] = w;
   wavelength[4] = w;
   wavelength[5] = w;
   wavelength[6] = w;
   wavelength[7] = w;

   float s = 0.1;
   speed[0] = s;
   speed[1] = s;
   speed[2] = s;
   speed[3] = s;
   speed[4] = s;
   speed[5] = s;
   speed[6] = s;
   speed[7] = s;

   vec2 dd = vec2(0.0, 0.01);
   direction[0] = dd;
   direction[1] = dd;
   direction[2] = dd;
   direction[3] = dd;
   direction[4] = dd;
   direction[5] = dd;
   direction[6] = dd;
   direction[7] = dd;

   //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
   
	vec4 pos = gl_Vertex;
    pos.z = waterHeight + waveHeight(pos.x, pos.y);
	
	final_position = gl_ModelViewProjectionMatrix * (gl_Vertex + pos*.035);
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
	
    //position = pos.xyz / pos.w;
    //worldNormal = waveNormal(pos.x, pos.y);
    //eyeNormal = gl_NormalMatrix * worldNormal;
	
	if (selected == true && wiggle == true)
		water();
	;
	
	gl_Position = final_position;
}
