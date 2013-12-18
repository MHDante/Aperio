#version 120
#extension GL_EXT_geometry_shader4 : enable
 
// a passthrough geometry shader for color and position
void main()
{
  for(int i = 0; i < gl_VerticesIn; ++i)
  {
    // copy color
    gl_FrontColor = gl_FrontColorIn[i];
    gl_BackColor = gl_BackColorIn[i];	
	gl_TexCoord[0] = gl_TexCoordIn[i][0];
	
    // copy position
    gl_Position = gl_PositionIn[i] + vec4(200,0,0,0);
 
    // done with the vertex
    EmitVertex();
  }
  EndPrimitive();
}
