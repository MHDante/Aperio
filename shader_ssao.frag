/******************************************************************
* Fragment Shader - Hemispherical Screen Space Ambient Occlusion 
* Edited by David Tran
* Adapted from:
* http://blog.evoserv.at/index.php/2012/12/hemispherical-screen-space-ambient-occlusion-ssao-for-deferred-renderers-using-openglglsl/
******************************************************************/

#version 440 compatibility

// Input from vs
smooth in vec4 vTexCoord;
vec2 vUv = vTexCoord.st;

// Output 
layout(location = 0) out vec4 oColor;

//---- Uniforms
uniform sampler2D source;
uniform sampler2D sourceNormal;
uniform sampler2D sourceDepth;
uniform sampler2D sourceCap;
uniform vec2 frameBufSize;

//uniform vec2 clipping = vec2(0.1, 800);

uniform mat4 projMat;
mat4 iprojMat = inverse(projMat);

bool onlyAO =false;

float zNear = 0.001;
float zFar = 2;
float cameraCoef = 2;

float distanceThreshold = 5;
vec2 filterRadius = vec2(10.0 / frameBufSize.x, 10.0 / frameBufSize.y);

const int sample_count = 16;			// 40 is good (16 won't slow)
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
vec2(-0.02355726f, -0.4306656f),
vec2(0.4161599f, -0.2374075f),
vec2(-0.07173875f, 0.05912307f),
vec2(0.422852f, -0.8331718f),
vec2(-0.4025845f, -0.6909922f),
vec2(-0.5639075f, 0.003217667f),
vec2(-0.0308353f, -0.9878358f),
vec2(0.3307182f, 0.1964717f),
vec2(0.9404664f, 0.04704203f),
vec2(-0.02944958f, 0.7125675f),
vec2(0.4827818f, 0.7235841f),
vec2(-0.3426326f, 0.4130895f),
vec2(-0.96206f, 0.1811415f),
vec2(-0.7418997f, 0.6424237f),
vec2(-0.8977579f, -0.2314493f),
vec2(0.7986656f, -0.4338621f)
);

float linearizeDepth(const in float depth) 
{
	return (cameraCoef * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

float UnpackFloat8bitRGB(vec3 pack) {
    return dot(pack, vec3(1.0, 1.0 / 255.0, 1.0 / 65025.0));
}

vec3 calculatePosition(const in vec2 coord)
{
	vec4 d = texture(sourceDepth, coord);
	
	float depth = UnpackFloat8bitRGB(d.rgb);	
	depth = linearizeDepth(depth);
  
	if (d.a <= 0)
    depth = 1.0;

	vec4 pos = iprojMat * vec4(coord.x * 2 - 1, coord.y * 2 - 1, depth * 2 - 1, 1);	
	pos /= pos.w;
	
	return pos.xyz;
}

vec3 calculateNormal(const in vec2 coord)
{
	vec4 normal = texture(sourceNormal, coord);
	
	if (normal.a <= 0)
		return vec3(0, 0, 1);
	else
		return normal.xyz * 2.0 - 1.0;
}

void main()
{
	// reconstruct position from depth
	vec3 viewPos = calculatePosition(vUv);

	// get the view space normal
	vec3 viewNormal = calculateNormal(vUv);

    float ambientOcclusion = 0;
    // perform AO
    for (int i = 0; i < sample_count; ++i)
    {
        // sample at an offset specified by the current Poisson-Disk sample and scale it by a radius (has to be in Texture-Space)
        vec2 sampleTexCoord = vUv + (poisson16[i] * (filterRadius));
		
        vec3 samplePos = calculatePosition(sampleTexCoord);
        vec3 sampleDir = normalize(samplePos - viewPos);

        // angle between SURFACE-NORMAL and SAMPLE-DIRECTION (vector from SURFACE-POSITION to SAMPLE-POSITION)
        float NdotS = max(dot(viewNormal, sampleDir), 0);
        // distance between SURFACE-POSITION and SAMPLE-POSITION
        float VPdistSP = distance(viewPos, samplePos);

        // a = distance function
        float a = 1.0 - smoothstep(distanceThreshold, distanceThreshold * 2, VPdistSP);
        // b = dot-Product
        float b = NdotS;

        ambientOcclusion += (a * b);
    }
	float ao = 1.0 - (ambientOcclusion / sample_count);	
	
	const vec4 onlyAOColor = vec4( 1.0, 0.7, 0.5, 1);		

	if (onlyAO)
		oColor = onlyAOColor * vec4(ao, ao, ao, 1);
	else
	{
		oColor = texture(source, vUv) * vec4(ao, ao, ao, 1);			
		
		// No AO on Capping Mask texture
		if (texture2D(sourceCap, vUv).r >= 1) 
			oColor = texture(source, vUv);
	}	
	//oColor = texture(sourceCap, vUv);			
}