#version 330 core

in vec3 vn;
in vec2 vt;

uniform sampler2D texture;

uniform vec4 color_e;

uniform int use_texture;

out vec4 col;


void main() {
  if (use_texture > 0) {
    col = color_e * texture2D( texture, vt );
  } else {
    col = color_e;
  }
}
