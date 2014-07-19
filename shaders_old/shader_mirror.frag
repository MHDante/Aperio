/*******************************************************************
	Fragment
	Mirror Shader : Mirror shader
*******************************************************************/

/**
 * @author felixturner / http://airtight.cc/
 *
 * Mirror Shader
 * Copies half the input to the other half
 *
 * side: side of input to mirror (0 = left, 1 = right, 2 = top, 3 = bottom)
 */

uniform sampler2D source;
int side = 0;

vec2 vUv = gl_TexCoord[0].st;

void main() {

	vec2 p = vUv;
	if (side == 0){
		if (p.x > 0.5) p.x = 1.0 - p.x;
	}else if (side == 1){
		if (p.x < 0.5) p.x = 1.0 - p.x;
	}else if (side == 2){
		if (p.y < 0.5) p.y = 1.0 - p.y;
	}else if (side == 3){
		if (p.y > 0.5) p.y = 1.0 - p.y;
	} 
	vec4 color = texture2D(source, p);
	gl_FragColor = color;

}