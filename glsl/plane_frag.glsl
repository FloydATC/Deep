#version 330 core
in vec2 vt;
in vec3 vn;

uniform sampler2D texture;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

uniform int is_debug;
uniform int use_texture;

out vec4 col;

float checkerboard(float x, float y, float scale) {
	return float((
		int(floor(x / scale)) +
		int(floor(y / scale))
	) % 2);
}


void main() {

  if (is_debug == 0 && use_texture > 0) {
    float c =
      checkerboard(vt.x, vt.y, 1) * 0.3 +
      checkerboard(vt.x, vt.y, 1/8.0) * 0.2 +
      checkerboard(vt.x, vt.y, 1/64.0) * 0.1 +
      0.1;
    col = color_d * vec4(vec3(c/2.0 + 0.3), 1);
  } else {
    col = vec4(1.0, 0.0, 1.0, 1.0);
  }
}
