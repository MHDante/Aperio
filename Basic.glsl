varying vec4 eyecoord;
varying vec3 N;
//varying vec3 vertex_light_position;

void main()
{   
	//vertex_light_position = normalize(gl_LightSource[0].position.xyz);

	gl_FrontColor = gl_Color;
	gl_BackColor = gl_Color;

	N = normalize(gl_NormalMatrix * gl_Normal);
	eyecoord = gl_ModelViewMatrix * gl_Vertex;
	
    gl_Position = ftransform();
} 