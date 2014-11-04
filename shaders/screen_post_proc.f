#include uniform_buffers.glsl
#include fxaa.glsl

in vec2 st;
out vec4 frag_color;
uniform sampler2D tex;
uniform sampler2D shadow_tex;

void main()
{
  //frag_color = fxaa(st, tex);
  frag_color.rgb = texture(tex, st).rgb;

  //vec3 shadow = texture(shadow_tex, st).rgb;
 // frag_color.rgb = shadow;
 /*
  float shadow = texture(shadow_tex, st).x;
  shadow = 1.0 - (1.0 - shadow) * 2;
  frag_color.rgb = vec3(shadow, shadow, shadow);
  */
  frag_color.a = 1.0; 
}



