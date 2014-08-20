/*******************************************************************
	Fragment
	Bloom Shader : Bloom Post-Process filter
*******************************************************************/

#version 440 compatibility

uniform sampler2D source;

uniform vec2 frameBufSize;
int samples = 6; // pixels per axis; higher = bigger glow, worse performance
float quality = 0.2; // lower = smaller glow, better quality
 
vec4 effect(vec4 colour, sampler2D tex, vec2 tc)
{
  vec4 source = texture2D(tex, tc);
  vec4 sum = vec4(0);
  int diff = (samples - 1) / 2;
  vec2 sizeFactor = vec2(1) / frameBufSize * quality;
  
  for (int x = -diff; x <= diff; x++)
  {
    for (int y = -diff; y <= diff; y++)
    {
      vec2 offset = vec2(x, y) * sizeFactor;
      sum += texture2D(tex, tc + offset);
    }
  }
  return ((sum / (samples * samples)) + source) * colour;
}

void main()
{
	gl_FragColor = effect(vec4(0.75, 0.675, 0.6, 1), source, gl_TexCoord[0].st);
  //gl_FragColor = texture2D(source, gl_TexCoord[0].st);
}