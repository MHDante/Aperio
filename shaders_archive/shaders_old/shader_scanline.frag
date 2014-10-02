/*******************************************************************
	Fragment
	Scanline Post-process Shader : Scanline and film shader
*******************************************************************/

/**
 * @author alteredq / http://alteredqualia.com/
 *
 * Film grain & scanlines shader
 *
 * - ported from HLSL to WebGL / GLSL
 * http://www.truevision3d.com/forums/showcase/staticnoise_colorblackwhite_scanline_shaders-t18698.0.html
 *
 * Screen Space Static Postprocessor
 *
 * Produces an analogue noise overlay similar to a film grain / TV static
 *
 * Original implementation and noise algorithm
 * Pat 'Hawthorne' Shearon
 *
 * Optimized scanlines + noise version with intensity scaling
 * Georg 'Leviathan' Steinrohder
 *
 * This version is provided under a Creative Commons Attribution 3.0 License
 * http://creativecommons.org/licenses/by/3.0/
 */

float time = 0.0;
bool grayscale = false;

// noise effect intensity value (0 = no effect, 1 = full effect)
float nIntensity = 0.5;

// scanlines effect intensity value (0 = no effect, 1 = full effect)
float sIntensity = 0.05;

// scanlines effect count value (0 = no effect, 4096 = full effect)
float sCount = 2048;

uniform sampler2D source;
vec2 vUv = gl_TexCoord[0].st;

void main() {

	// sample the source
	vec4 cTextureScreen = texture2D( source, vUv );

	// make some noise
	float x = vUv.x * vUv.y * time *  1000.0;
	x = mod( x, 13.0 ) * mod( x, 123.0 );
	float dx = mod( x, 0.01 );

	// add noise
	vec3 cResult = cTextureScreen.rgb + cTextureScreen.rgb * clamp( 0.1 + dx * 100.0, 0.0, 1.0 );

	// get us a sine and cosine
	vec2 sc = vec2( sin( vUv.y * sCount ), cos( vUv.y * sCount ) );

	// add scanlines
	cResult += cTextureScreen.rgb * vec3( sc.x, sc.y, sc.x ) * sIntensity;

	// interpolate between source and result by intensity
	cResult = cTextureScreen.rgb + clamp( nIntensity, 0.0,1.0 ) * ( cResult - cTextureScreen.rgb );

	// convert to grayscale if desired
	if( grayscale ) {

		cResult = vec3( cResult.r * 0.3 + cResult.g * 0.59 + cResult.b * 0.11 );

	}

	gl_FragColor =  vec4( cResult, cTextureScreen.a );

}
