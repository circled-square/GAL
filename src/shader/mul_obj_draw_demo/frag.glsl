#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out int dummy;

uniform vec4 u_color;
uniform sampler2D u_texture_slot;

in vec2 v_tex_coord;

void main() {
   vec2 normalized_pos = (v_tex_coord - vec2(.5, .5)) * 2;
   if(length(normalized_pos) > 1)
      color = vec4(0,0,0,0);//(u_color + tex_color) / 2.f;
   else {
      vec4 tex_color = texture(u_texture_slot, v_tex_coord);
      color = (u_color + tex_color) / 2.f;
   }
}
