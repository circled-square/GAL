#version 440 core

layout(location = 0) in vec2 pos;
layout(location = 1) in float angle;

#define PI 3.14159265358979323846264

void main() {
   float len = (pos.xy).length() / 4.0;
   float pos_atan = atan(pos.y / pos.x);
   float pos_angle = pos_atan + (pos_atan < 0 ? PI : 0) + (pos.y < 0 ? PI : 0);


   vec4 outpos = vec4(
      len * cos(pos_angle + angle),
      len * sin(pos_angle + angle),
      0, 1
   );

   gl_Position = outpos;
}





