/*******************************************************************
	Fragment
	Bloom Shader : Bloom Post-Process filter
*******************************************************************/

#version 440 compatibility

uniform sampler2D source;
 
const float BRIGHT_PASS_THRESHOLD = 0.5;
const float BRIGHT_PASS_OFFSET = 1.75;
 
#define blurclamp 1.0
#define bias 0.025
 
#define KERNEL_SIZE  4.0
 
vec2 vUv = gl_TexCoord[0].st;
 
vec4 bright(vec2 coo)
{
    vec4 color = texture2D(source, coo);
     
    color = max(color - BRIGHT_PASS_THRESHOLD, 0.0);
     
    return color / (color + BRIGHT_PASS_OFFSET);    
}
 
 
void main(void)
{
    vec2 blur = vec2(clamp( bias, -blurclamp, blurclamp ));
     
    vec4 col = vec4( 0, 0, 0, 0 );
    for ( float x = -KERNEL_SIZE + 1.0; x < KERNEL_SIZE; x += 1.0 )
    {
    for ( float y = -KERNEL_SIZE + 1.0; y < KERNEL_SIZE; y += 1.0 )
    {
         col += bright( vUv + vec2( blur.x * x, blur.y * y ) );
    }
    }
    col /= ((KERNEL_SIZE+KERNEL_SIZE)-1.0)*((KERNEL_SIZE+KERNEL_SIZE)-1.0);
    gl_FragColor = col + texture2D(source, vUv);
}