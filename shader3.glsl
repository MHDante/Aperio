varying vec3 N;
varying vec3 v;

varying vec3 vv;

void main(void)  
{     
   v = vec3(gl_ModelViewMatrix * gl_Vertex);       
   vv = vec3(gl_Vertex);       

   //v = vec3(gl_Vertex);       
   N = normalize(gl_NormalMatrix * gl_Normal);
   
   gl_FrontColor = gl_Color;
   gl_BackColor = gl_Color;
   
   float sx = 1;
   float sy = 1;
   
   // column-based, NOT row-based
   mat4 scaling;
   scaling[0] = vec4(sx, 0, 0, 0);
   scaling[1] = vec4(0, sy, 0, 0);
   scaling[2] = vec4(0, 0, 1, 0);
   scaling[3] = vec4(0, 0, 0, 1);
   
   gl_Position = gl_ModelViewProjectionMatrix * (scaling * gl_Vertex );  
}