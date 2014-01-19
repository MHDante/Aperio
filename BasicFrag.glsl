uniform mat4 mat;
uniform vec3 mouse;
varying vec4 eyecoord;
varying vec3 N;
//varying vec3 vertex_light_position;

uniform float mouseSize;
uniform float translucency;
uniform int selected;
uniform int peerInside;

/* Compute the contribution from a particular light source.  This basically
 * comes straight out of the OpenGL orange book. */
void DirectionalLight(in int lightIndex,
                      in vec3 normal,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular)
{
  /**** Compute ambient term. ****/
  ambient += gl_LightSource[lightIndex].ambient;

  /**** Compute diffuse term. ****/
  /* normal dot light direction.  Assume the light direction vector is already
     normalized.*/
  float nDotL = max(0.0, dot(normal,
                             normalize(vec3(gl_LightSource[lightIndex].position))));
  diffuse += gl_LightSource[lightIndex].diffuse * nDotL;

  /**** Compute specular term. ****/
  /* normal dot halfway vector */
  float nDotH = max(0.0, dot(normal,
                             vec3(gl_LightSource[lightIndex].halfVector)));
  float pf;     /* Power factor. */
  if (nDotH >= 0.0)
    {
    pf = 0.0;
    }
  else
    {
    pf = pow(nDotH, gl_FrontMaterial.shininess);
    }
  specular += gl_LightSource[lightIndex].specular * pf;
}

void main()
{   
	// no use for translucency yet
	float s = translucency;
	
	vec4 pos = eyecoord;
	vec4 mpos = mat * vec4(mouse, 1);

	float size = mouseSize;

	float dist = (mpos.x - pos.x)*(mpos.x - pos.x) +
	(mpos.y - pos.y)*(mpos.y - pos.y);

	vec4 ambient = vec4(0.0);
	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);

	DirectionalLight(0, N, ambient, diffuse, specular);
	
	vec4 myFragColor = (  ambient*gl_FrontMaterial.ambient + diffuse*gl_FrontMaterial.diffuse + specular*gl_FrontMaterial.specular);

	if (dist <= size*size )
	{
		if (peerInside == 1)
		{
			if (dist <= size*size * 0.8)
				discard;
		}	
		gl_FragColor = vec4(myFragColor) - vec4(0.2, 0.2, 0.2, 0);
	}	
	else
	{
		// normal
		gl_FragColor = myFragColor;
		
		if (selected == 1)
		{
			gl_FragColor = vec4(myFragColor) + vec4(0.4f, 0, 0, 0);
		
		}

	}
		
} 