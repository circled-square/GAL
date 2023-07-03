#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D u_texture_slot;

in vec2 v_tex_coord;

void main() {
    color = texture(u_texture_slot, v_tex_coord);
}
