/*******************************************************************
	Fragment
	Kaleido Shader : Kaleidoscope shader
*******************************************************************/

/**
 * @author felixturner / http://airtight.cc/
 *
 * Kaleidoscope Shader
 * Radial reflection around center point
 * Ported from: http://pixelshaders.com/editor/
 * by Toby Schachman / http://tobyschachman.com/
 *
 * sides: number of reflections
 * angle: initial angle in radians
 */
 
uniform sampler2D source;
float sides = 12.0;
float angle = 0.0;

vec2 vUv = gl_TexCoord[0].st;

void main() {

	vec2 p = vUv - 0.5;
	float r = length(p);
	float a = atan(p.y, p.x) + angle;
	float tau = 2. * 3.1416 ;
	a = mod(a, tau/sides);
	a = abs(a - tau/sides/2.) ;
	p = r * vec2(cos(a), sin(a));
	vec4 color = texture2D(source, p + 0.5);
	gl_FragColor = color;

}