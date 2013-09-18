
[Vertex Shader]
// Shader OpenGL ID 5
varying vec3 position, normal;

void main(void)
{
	normal = normalize(gl_NormalMatrix * gl_Normal);
	position = normalize((gl_ModelViewMatrix * gl_Vertex).xyz);
	
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
[Fragment Shader]
// Shader OpenGL ID 6
varying vec3 position, normal;

float fmod(float x, float y)
{
	return x - y * trunc(x/y);
}

void main()
{
//if (normal.y < 0.011)
//	gl_FragColor = vec4(0, 0, 0, 1);
//else
//	gl_FragColor = gl_Color;
	
	gl_FragColor = vec4(1, 1, 1, 1) * normal.x;
}


