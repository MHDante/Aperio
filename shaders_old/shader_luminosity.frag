/*******************************************************************
	Fragment
	Luminosity Shader : Luminosity shader
*******************************************************************/

/**
 * @author alteredq / http://alteredqualia.com/
 *
 * Luminosity
 * http://en.wikipedia.org/wiki/Luminosity
 */

uniform sampler2D source;
vec2 vUv = gl_TexCoord[0].st;

void main() {

	vec4 texel = texture2D( source, vUv );

	vec3 luma = vec3( 0.299, 0.587, 0.114 );

	float v = dot( texel.xyz, luma );

	//gl_FragColor = vec4( v, v, v, texel.w );
	gl_FragColor = texel;

}