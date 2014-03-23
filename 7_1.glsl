
[Vertex Shader]
// Shader OpenGL ID 2
/*******************************************************************
	Vertex
	Main Shader : Phong Reflection model and main interaction	
*******************************************************************/

//#version 130
//#extension GL_EXT_geometry_shader4 : enable

varying vec3 n;
varying vec3 v;
varying vec3 original_v;

void main(void)  
{     
   v = vec3(gl_ModelViewMatrix * gl_Vertex);       
   n = normalize(gl_NormalMatrix * gl_Normal);

   original_v = vec3(gl_Vertex);       
	
   gl_FrontColor = gl_Color;
   gl_BackColor = gl_Color;
   gl_TexCoord[0] = gl_MultiTexCoord0;   
	
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}
[Fragment Shader]
// Shader OpenGL ID 3
/*******************************************************************
	Fragment
	Main Shader : Phong Reflection model and main interaction	
*******************************************************************/

varying vec3 n;
varying vec3 v;    
varying vec3 original_v;

uniform vec3 mouse;
uniform float mouseSize;
uniform int selected;
uniform int peerInside;

uniform float myexp;
uniform sampler2D mytext;

uniform int shadingnum;

// Global variables
vec4 final_color;



// ---------------- Toon Shader ----------------------//

void toon()
{
	const int levels = 5;
	const float scaleFactor = 1.0 / levels; 
	
	vec4 col = gl_Color;

	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0) surf2Eye  
	vec3 L = normalize(gl_LightSource[0].position.xyz);   // surf2Light
	vec3 R = normalize(-reflect(L,n));  // Reflection of surf2Light and normal
	
	float intensity = max(dot(n, normalize(L)), 0.0);
	
	vec3 diffuse = gl_FrontMaterial.diffuse * floor(intensity * levels) * scaleFactor;
	
	final_color = gl_FrontMaterial.ambient / 20.0f + vec4(diffuse, 1);
}

// ---------------- Subsurface Scatter Shader (Approximate) ----------------------//
float halfLambert(in vec3 vect1, in vec3 vect2)
{
    float product = dot(vect1,vect2);
    return product * 0.5 + 0.5;
}
float blinnPhongSpecular(in vec3 normalVec, in vec3 lightVec, in float specPower)
{
    vec3 halfAngle = normalize(normalVec + lightVec);
    return pow(clamp(0.0,1.0,dot(normalVec,halfAngle)),specPower);
}
// Main fake sub-surface scatter lighting function
void subScatterFS()
{
	// Variables for lighting properties
	float MaterialThickness = 0.6f;
	vec3 ExtinctionCoefficient = vec3(0.8f, 0.2f, 0.12f); // Will show as X Y and Z ports in QC, but actually represent RGB values.
	vec4 LightColor = vec4(.2, .2, .2, 1);
	vec4 BaseColor = gl_Color * 7.0f;//vec4(0.5f, 0.5f, 0.5f, 1);
	vec4 SpecColor = vec4(-0.4, -0.4, -0.4, 1);
	float SpecPower =0.0f;
	float RimScalar = 10.0f;
	//uniform sampler2D Texture;
	 
	// Varying variables to be sent to Fragment Shader
	vec3 eyeVec = -v;
	vec3 lightPos = gl_LightSource[0].position;
	vec3 lightVec = lightPos - v.xyz;

    float attenuation = 0.8f;//10.0 * (1.0 / distance(lightPos,v));
    vec3 eVec = normalize(eyeVec);
    vec3 lVec = normalize(lightVec);
    vec3 wNorm = normalize(n);
     
    vec4 dotLN = vec4(halfLambert(lVec,wNorm) * attenuation);
    //dotLN *= texture2D(Texture, gl_TexCoord[0].xy);
    dotLN *= BaseColor;
     
    vec3 indirectLightComponent = vec3(MaterialThickness * max(0.0,dot(-wNorm,lVec)));
    indirectLightComponent += MaterialThickness * halfLambert(-eVec,lVec);
    indirectLightComponent *= attenuation;
    indirectLightComponent.r *= ExtinctionCoefficient.r;
    indirectLightComponent.g *= ExtinctionCoefficient.g;
    indirectLightComponent.b *= ExtinctionCoefficient.b;
     
    vec3 rim = vec3(1.0 - max(0.0,dot(wNorm,eVec)));
    rim *= rim;
    rim *= max(0.0,dot(wNorm,lVec)) * SpecColor.rgb;
     
    final_color = dotLN + vec4(indirectLightComponent,1.0);
    final_color.rgb += (rim * RimScalar * attenuation * final_color.a);
    final_color.rgb += vec3(blinnPhongSpecular(wNorm,lVec,SpecPower) * attenuation * SpecColor * final_color.a * 0.05);
    final_color.rgb *= LightColor.rgb;
     
	final_color.a = gl_Color.a;
}

// ***************-------------------- Main function -------------------------***************//
void main (void)  
{
	if (!gl_FrontFacing) 
		n = -n;

	if (shadingnum == 0)
		subScatterFS();
	else if (shadingnum == 1)
		toon();
	else
		phongLighting(0);	
		
	// convert mouse world coords to view coords (so same as v)
	vec4 mouseV = gl_ModelViewMatrix * vec4(vec3(mouse), 1);
	//vec4 mouseV = vec4(vec3(mouse), 1);
	
	float ee = myexp;
	float nn = 0.5;

	float d = pow(pow(abs(v.y - mouseV.y) / 1.0, 2.0/ee) + pow(abs(v.x - mouseV.x) / 1.0, 2.0/ee), ee/nn) + pow(abs(v.z - mouseV.z) / 1.0, 2.0/nn) - 1.0;
	//float d = pow(pow(abs(original_v.y - mouseV.y) / 1.0, 2.0/ee) + pow(abs(original_v.x - mouseV.x) / 1.0, 2.0/ee), ee/nn) + pow(abs(original_v.z - mouseV.z) / 1.0, 2.0/nn) - 1.0;
	  
	if (selected == 1)	// if selected
	{
		if (d < mouseSize && peerInside == 1)
		{
			discard;
		}
		else
		{
			gl_FragColor = final_color;

		}
	}
	else
	{
			gl_FragColor = final_color;
	}
}