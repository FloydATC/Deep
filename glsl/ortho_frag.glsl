#version 330 core

in vec3 vn;
in vec2 vt;

uniform sampler2D texture;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

uniform int is_debug;
uniform int use_texture;

out vec4 col;


void main() {

  if (is_debug > 0) {
    col = vec4(1.0, 0.0, 0.0, 1.0) + vec4(vn.x, vn.y, vn.z, 1.0);
  } else {
    if (use_texture > 0) {
      col = color_e * texture2D( texture, vt );
    } else {
      col = color_e;
    }
  }
}
