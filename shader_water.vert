/*******************************************************************
	Vertex
	Water Shader : Rippling water shader
*******************************************************************/

#version 420 compatibility

// Output to fs
out vec3 n;
out vec3 v;
out vec3 original_v;

out vec4 vTexCoord;

//--- Uniforms
uniform float time = 0.0;
uniform bool selected = false;
uniform bool iselem = false;
uniform bool wiggle = false;

//--- Superquad data
uniform vec3 pos1 = vec3(0, 0, 0);
uniform vec3 pos2 = vec3(0, 0, 0);
uniform vec3 norm1 = vec3(0, 0, 0);
uniform vec3 norm2 = vec3(0, 0, 0);
uniform vec3 scale = vec3(0, 0, 0);

uniform float phi = 0.0;
uniform float theta = 0.0;
uniform float thickness = 0.4;
uniform bool toroid = false;

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
	//float Power = mod(time, 1.0) * 1.0f;
	float Power =  1.0f;
    radius = pow(radius, Power);

    // Convert back to Cartesian:
    v.x = radius * cos(theta);
    v.y = radius * sin(theta);
    p.xy = v.xy * p.w;
    return p;
}

vec3 opCheapBend( vec3 p )
{
	if (!iselem)
		return p;
		
	float dist = distance(pos1, pos2);
	vec3 poss = (pos1 + pos2) / 2.0;
		
	// Rotation matrix
	vec3 right = normalize(pos2 - pos1);	
	//vec3 forward = normalize((norm1 + norm2) / 2.0);
	//vec3 up = normalize(cross(forward, right));

	vec3 up = normalize((norm1 + norm2) / 2.0);
	vec3 forward = normalize(cross(right, up));

	mat3 rotMat = mat3(
	right, up, forward
	);
	vec3 o = inverse(rotMat) * (p - poss);
	//vec3 o = inverse(rotMat);// * (p - poss);	
	
	
    /*float c = cos(mod(time, 1.0) * radians(50.0)*o.y);
    float s = sin(mod(time, 1.0) * radians(50.0)*o.y);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*o.xy,o.z);*/
    
	/*o = 
	mat3(
	vec3(1, cos(ang),-sin(ang) ),
	vec3(0, sin(ang),cos(ang)  ),
	vec3(0, 0,1)
	) * o + vec3(0, 0, 1) ;*/
	
	float ang = 0;
	
	if (o.x > -1)
		ang = -70;
	//if (o.x > 0.1)
	float c = cos( radians(ang)*o.x);
    float s = sin( radians(ang)*o.x);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*o.xy,o.z);
	
	vec3 ff = (rotMat * q) + poss;
	
	return ff;
	 //q = poss;
	
}

vec3 opTwist( vec3 p )
{
    float c = cos(mod(time, 1.0) * radians(20.0)*p.y);
    float s = sin(mod(time, 1.0) * radians(20.0)*p.y);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xz,p.y);
    return q;
}

vec3 bend(vec3 p)
{
	if (!iselem)
		return p;
	
	float dist = distance(pos1, pos2);
	vec3 poss = (pos1 + pos2) / 2.0;
		
	// Rotation matrix
	vec3 right = normalize(pos2 - pos1);	
	//vec3 forward = normalize((norm1 + norm2) / 2.0);
	//vec3 up = normalize(cross(forward, right));

	vec3 up = normalize((norm1 + norm2) / 2.0);
	vec3 forward = normalize(cross(right, up));

	mat3 rotMat = mat3(
	right, up, forward
	);
	vec3 o = inverse(rotMat) * (p - poss);
	o = o + vec3(0.5 * dist, -0.1, 0);
	
	vec3 q;	
	float pi = 3.141592654f;
	//float r = (dist / (2 * pi));
	//float r = 2 * dist;
	float r = 5;

	q = o;
	
	if (o.x < r * pi)
	{
		q.x = -o.y * sin( (o.x / r)  );
		q.y = o.y * cos( (o.x / r) );
		q.z = o.z;
	}
	// preserved?
	else
	{
		q.x = -(o.x - r * pi);
		q.y = -o.y;
		q.z = o.z;
	}
	
	return q;
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
	
	//final_position = gl_ModelViewProjectionMatrix * vec4((gl_Vertex.rgb), 1);
	
	gl_Position = final_position ;
}
