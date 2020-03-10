#version 330 core
in vec2 vt;
in vec3 vn;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_bump;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

uniform int is_debug;
uniform int use_texture;

out vec4 col;



void main() {

  if (is_debug > 0) {
    col = vec4(1.0, 0.0, 0.0, 1.0);
  } else {
    if (use_texture > 0) {
      col = color_d * texture2D( texture_diffuse, vt );
    } else {
      col = color_d;
    }
  }
}
