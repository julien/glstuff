#version 410

in vec2 v_position;
in float v_pointsize;

out vec4 frag_color;

uniform vec2 u_resolution;
uniform float u_time;

void main() { frag_color = vec4(1.0, 1.0, 1.0, 0.4); }
