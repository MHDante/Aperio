/******************************************************************
* Fragment Shader - Hemispherical Screen Space Ambient Occlusion 
* Edited by David Tran
* Adapted from:
* http://blog.evoserv.at/index.php/2012/12/hemispherical-screen-space-ambient-occlusion-ssao-for-deferred-renderers-using-openglglsl/
******************************************************************/

#version 440 compatibility

smooth in vec2 vUv;

uniform sampler2D source;
uniform sampler2D sourceNormal;
uniform sampler2D sourceDepth;
uniform vec2 frameBufSize;

uniform mat4 projMat;
mat4 iprojMat = inverse(projMat);

bool onlyAO =false;

float zNear = 0.015;
float zFar = 100;
float cameraCoef = 2;

float distanceThreshold = 0.03;
vec2 filterRadius = vec2(10.0 / frameBufSize.x, 10.0 / frameBufSize.y);

const int sample_count = 0;			// 40 is good
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
vec2(-0.2918438f, 0.7097818f),
vec2(-0.5697373f, 0.3234968f),
vec2(0.08700638f, 0.5978211f),
vec2(-0.6084387f, 0.5826824f),
vec2(-0.5492917f, 0.8244911f),
vec2(-0.2969716f, 0.2535218f),
vec2(-0.1380409f, 0.505855f),
vec2(-0.09842424f, 0.9828971f),
vec2(0.1446213f, 0.8812384f),
vec2(0.0300821f, 0.3272773f),
vec2(0.417667f, 0.7309052f),
vec2(0.3703419f, 0.3500031f),
vec2(-0.07869275f, 0.03726918f),
vec2(0.2422249f, -0.07930708f),
vec2(-0.7164432f, 0.03987473f),
vec2(-0.8841425f, 0.4390802f),
vec2(-0.4693542f, -0.08020838f),
vec2(-0.957849f, 0.1858903f),
vec2(0.5524602f, -0.1926276f),
vec2(-0.1098147f, -0.2047909f),
vec2(0.1686378f, -0.4164315f),
vec2(0.5009063f, 0.06640533f),
vec2(0.5136947f, -0.4606736f),
vec2(-0.03520988f, -0.5709935f),
vec2(-0.4799206f, -0.4785594f),
vec2(-0.2276017f, -0.4257426f),
vec2(0.8122018f, -0.3806681f),
vec2(0.7720947f, 0.01240732f),
vec2(0.6623059f, 0.4374966f),
vec2(0.6804902f, 0.7129371f),
vec2(0.3109443f, -0.8586086f),
vec2(0.03161683f, -0.8538675f),
vec2(-0.4194146f, -0.7615957f),
vec2(-0.6958252f, -0.3399802f),
vec2(-0.9348733f, -0.1859573f),
vec2(0.9477305f, 0.2047868f),
vec2(0.9574907f, -0.1537475f),
vec2(-0.6839828f, -0.6736622f),
vec2(-0.2138845f, -0.9265943f),
vec2(0.6941394f, -0.6445384f)
);

float linearizeDepth(const in float depth) 
{
	return (cameraCoef * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

vec3 calculatePosition(const in vec2 coord)
{
	float depth = texture(sourceDepth, coord).r;
	depth = linearizeDepth(depth);

	vec4 pos = iprojMat * vec4(coord.x * 2 - 1, coord.y * 2 - 1, depth * 2 - 1, 1);	
	pos /= pos.w;
	
	return pos.xyz;
}
	
void main()
{
	// reconstruct position from depth
	vec3 viewPos = calculatePosition(vUv);

	// get the view space normal
	vec3 viewNormal = texture(sourceNormal, vUv).xyz * 2.0 - 1.0;

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
		gl_FragColor = onlyAOColor * vec4(ao, ao, ao, 1);
	else
		gl_FragColor = texture(source, vUv) * vec4(ao, ao, ao, 1);	
		
	//gl_FragColor = texture(source, vUv);
}