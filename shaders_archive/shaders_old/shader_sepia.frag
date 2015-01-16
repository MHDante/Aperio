/*******************************************************************
	Fragment
	Sepia Shader : Sepia Shader
*******************************************************************/

/**
 * @author alteredq / http://alteredqualia.com/
 *
 * Sepia tone shader
 * based on glfx.js sepia shader
 * https://github.com/evanw/glfx.js
 */
 
float amount = 1.0;
uniform sampler2D source;

vec2 vUv = gl_TexCoord[0].st;

void main() {

	vec4 color = texture2D( source, vUv );
	vec3 c = color.rgb;

	color.r = dot( c, vec3( 1.0 - 0.607 * amount, 0.769 * amount, 0.189 * amount ) );
	color.g = dot( c, vec3( 0.349 * amount, 1.0 - 0.314 * amount, 0.168 * amount ) );
	color.b = dot( c, vec3( 0.272 * amount, 0.534 * amount, 1.0 - 0.869 * amount ) );

	gl_FragColor = vec4( min( vec3( 1.0 ), color.rgb ), color.a );

}
