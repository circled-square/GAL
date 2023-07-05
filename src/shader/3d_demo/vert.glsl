#version 440 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex_coord;

out vec2 v_tex_coord;
out float dbg;

uniform mat4 u_mvp; //model view projection matrix

void main() {
   //gl_Position = u_mvp * vec4(pos.xyz, 1);
   //gl_Position.w = gl_Position.z = 1;
   gl_Position = u_mvp * vec4(pos, 1);
   dbg = gl_Position.z;
   v_tex_coord = tex_coord;
}





