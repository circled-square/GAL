#version 440 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_mvp; //model view projection matrix

void main() {
   gl_Position = u_mvp * vec4(pos, 0, 1);
   v_tex_coord = tex_coord;
}





