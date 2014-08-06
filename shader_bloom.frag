/*******************************************************************
	Fragment
	Bloom Shader : Bloom Post-Process filter
*******************************************************************/

uniform sampler2D source;

void main()
{  
   vec4 finalcolor;

   vec4 sum = vec4(0);
   vec2 texcoord = vec2(gl_TexCoord[0]);
  
   for(int i= 0 ; i < 8; i++)
   {
        for (int j = 0; j < 8; j++)
        {
			//sum += texture2D(source, texcoord + vec2(j-3, i-3)*0.004) * 0.075;
			sum += texture2D(source, texcoord + vec2(j-3, i-3)*0.008) * 0.065;
        }
   }
   
    if (texture2D(source, texcoord).r < 0.3)
    {
       finalcolor = sum*sum*0.012 + texture2D(source, texcoord);
    }
    else
    {
        if (texture2D(source, texcoord).r < 0.5)
        {
            finalcolor = sum*sum*0.009 + texture2D(source, texcoord);
        }
        else
        {
            finalcolor = sum*sum*0.0075 + texture2D(source, texcoord);
        }
    }
	
	gl_FragColor = finalcolor;
}