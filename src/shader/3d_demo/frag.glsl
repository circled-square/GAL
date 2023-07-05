#version 330 core

out vec4 color;

uniform sampler2D u_texture_slot;

in vec2 v_tex_coord;
in float dbg;

void main() {
   color = texture(u_texture_slot, v_tex_coord);
   //color =vec4(v_tex_coord, .3, 1.);
   //color.z = dbg;
}
