#version 330

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 view;
};

layout(std140) uniform Matrices {
  mat4 model;
};

uniform sampler2D position_tex;
uniform sampler2D normal_tex;

in vec2 st;

out vec4 frag_color;

void main () 
{
  vec4 p_texel = texture2D(position_tex, st);
  vec4 n_texel = texture2D(normal_tex, st);

  frag_color.rgb =  p_texel.rgb;
  //frag_color.rgb = normalize(n_texel.rgb);
 // frag_color.rgb = vec3(0, 1, 0);
  frag_color.a = 1.0;
}
