#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 6) in vec2 texture_coord;

out vec2 st;

void main()
{
  vec2 viewport = vec2(1280, 720);

  st = texture_coord;
  //st = (vec2(vertex_position) + 1.0) * 0.5;
  gl_Position = vec4(vec2(vertex_position) * 2.0 / viewport - 1.0, 0.0, 1.0);
}


