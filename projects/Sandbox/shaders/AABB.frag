/*
   Helps rendering a light position in the camera view for debugging
   @author Roberto Cano
*/
#version 450 core

uniform vec3 BoxColor;
out vec4 o_color;

void main() { o_color = vec4(BoxColor, 1.0f); }