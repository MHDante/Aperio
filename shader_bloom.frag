/*******************************************************************
	Fragment
	Bloom Shader : Bloom Post-Process filter
*******************************************************************/

#version 450 compatibility

uniform sampler2D source;
 
const float BRIGHT_PASS_THRESHOLD = 0.5;	// 0.5 
const float BRIGHT_PASS_OFFSET = 2.5;		// 0.8
 
const float blurclamp = 0.003;	// 0.003
const float bias = 0.01;		// 0.01
 
float KERNEL_SIZE = 3;		// 5 (3 won't slow down)
 
vec4 bright(const in vec2 coord)
{
    vec4 color = texture2D(source, coord);     
    color = max(color - BRIGHT_PASS_THRESHOLD, 0.0);
     
    return color / (color + BRIGHT_PASS_OFFSET);    
}
 
void main(void)
{
    vec2 blur = vec2(clamp( bias, -blurclamp, blurclamp));     
    vec4 col = vec4(0, 0, 0, 0);

    for (float x = -KERNEL_SIZE + 1.0; x < KERNEL_SIZE; x += 1.0 )
	{
		for (float y = -KERNEL_SIZE + 1.0; y < KERNEL_SIZE; y += 1.0 )
		{
			 col += bright(gl_TexCoord[0].st + vec2( blur.x * x, blur.y * y ));
		}
	}
    col /= ((KERNEL_SIZE+KERNEL_SIZE)-1.0)*((KERNEL_SIZE+KERNEL_SIZE)-1.0);
    gl_FragColor = col + texture2D(source, gl_TexCoord[0].st);
}