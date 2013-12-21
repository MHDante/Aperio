#version 120
//#extension GL_EXT_geometry_shader4 : enable

uniform sampler2D mytext;

void main()
{
//gl_FragColor = texture2D(mytext, gl_TexCoord[0].st);
//return;

   vec4 sum = vec4(0);
   vec2 texcoord = vec2(gl_TexCoord[0]);
   int j;
   int i;

   for( i= -3 ;i < 3; i++)
   {
        for (j = -4; j < 4; j++)
        {
			//sum += texture2D(mytext, texcoord + vec2(j, i)*0.004) * 0.25;
			sum += texture2D(mytext, texcoord + vec2(j, i)*0.004) * 0.20;
        }
   }
   
    if (texture2D(mytext, texcoord).r < 0.3)
    {
       gl_FragColor = sum*sum*0.012 + texture2D(mytext, texcoord);
    }
    else
    {
        if (texture2D(mytext, texcoord).r < 0.5)
        {
            gl_FragColor = sum*sum*0.009 + texture2D(mytext, texcoord);
        }
        else
        {
            gl_FragColor = sum*sum*0.0075 + texture2D(mytext, texcoord);
			//gl_FragColor = sum*sum*1+ texture2D(mytext, texcoord);
        }
    }
}