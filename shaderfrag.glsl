varying vec3 N;
varying vec3 v;    
varying vec3 vv;    

uniform vec3 mouse;
uniform float mouseSize;
uniform int peerInside;
uniform int selected;

uniform vec3 pos1;
uniform vec3 pos2;

uniform float myexp;

float evaluate(vec3 planeNorm, vec3 planeOrigin, vec3 v)
{
	return 	planeNorm.x * (v.x - planeOrigin.x) + 
			planeNorm.y * (v.y - planeOrigin.y) + 
			planeNorm.z * (v.z - planeOrigin.z);
}

void main (void)  
{  
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)  
	vec3 R = normalize(-reflect(L,N));  

	//calculate Ambient Term:  
	vec4 Iamb = gl_FrontLightProduct[0].ambient;    

	//calculate Diffuse Term:  
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);     

	// calculate Specular Term:
	vec4 Ispec = gl_FrontLightProduct[0].specular 
	* pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
	Ispec = clamp(Ispec, 0.0, 1.0); 

	// Calculate final color
	vec4 final_color = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;

	// ----- Start Custom code ----- 

	// convert mouse world coords to view coords (so same as v)
	vec4 mouseV = gl_ModelViewMatrix * vec4(vec3(mouse), 1);
	//vec4 mouseV = vec4(vec3(mouse), 1);
	
	vec4 mouseV2 = gl_ModelViewMatrix * vec4(vec3(mouse) + vec3(1,0,0), 1);
	
	//int p = 1000;
	
	float d = pow(abs((v.y - mouseV.y) / 1), myexp) + pow(abs((v.x - mouseV.x) / 2), myexp) + pow(abs((v.z - mouseV.z) / 1), myexp);	

	//float d = pow(abs(v.y - mouseV.y), p) + pow(abs(v.x - mouseV.x), p) + pow(abs(v.z - mouseV.z), p);	


	//float d = sqrt((v.y - mouseV.y)*(v.y - mouseV.y) + (v.x - mouseV.x)*(v.x - mouseV.x) );	
	
	if (selected == 1)	// if selected
	{
		// Plane Normal and 
		float lineWidth = 0.05;
		float lineLength = 0.9;
		
		vec3 surfaceNorm = gl_NormalMatrix * vec3(0, 0, 1);
		
		// Convert plane origin to view coordinates
		vec3 planeOriginModel = vec3(		
		pos1.x,
		pos1.y,
		pos1.z
		//0.3989804,
		//54.9921,
		//1.76695);
		);

		vec3 planeNorm = gl_NormalMatrix * vec3(1, 0, 0);	
		vec3 planeNormN = normalize(planeNorm);		
		vec3 planeOrigin = gl_ModelViewMatrix * vec4(vec3(planeOriginModel), 1);
		//vec3 planeOrigin = planeOriginModel;

		// crossproduct of surface normal and plane normal
		// Perpendicular plane of plane Normal
		vec3 pPlaneNorm = cross(surfaceNorm, planeNorm);
		vec3 pPlaneNormN = normalize(pPlaneNorm);
		
		float resultF = evaluate(planeNormN, planeOrigin + planeNormN*lineWidth, v);
		float resultB = evaluate(planeNormN, planeOrigin - planeNormN*lineWidth, v);
	
		float pResultF = evaluate(pPlaneNormN, planeOrigin + pPlaneNormN*lineLength/2.0, v);

		float pResultB = evaluate(pPlaneNormN, planeOrigin - pPlaneNormN*lineLength/2.0, v);	
		
		// Mystuff
		//if (d < mouseSize*mouseSize*mouseSize*0.8)
		if (d < mouseSize)
		{
			discard;
		}
		//if (d <= mouseSize*mouseSize)
		//if (resultt < 0)
		if (pResultF < 0 && pResultB > 0 && resultF < 0 && resultB > 0)
		//if (v.x > mouseV.x)
		{	
			//if (d < mouseSize*mouseSize*0.8)
			{
				if (peerInside == 1)
					discard;
			}
				
			vec4 new_color = vec4(final_color) - 0.3;
			//new_color.x += 0.3;
			gl_FragColor = vec4(new_color);
		}
		else
		{
			vec4 new_color = vec4(final_color) + vec4(.4, 0, 0, 0);
			gl_FragColor = vec4(vec3(new_color), gl_Color.a);     
		}
	}
	else
	{
		gl_FragColor = vec4(vec3(final_color), gl_Color.a);     
	}
}