#version 330 core
in vec3 v;
in vec2 vt;
in vec3 vn;
in vec3 eye;

uniform sampler2D texture;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

vec3 light_position = vec3( -0.20, 1.00, 1.50);

out vec4 col;

float checkerboard(float x, float y, float scale) {
	return float((
		int(floor(x / scale)) +
		int(floor(y / scale))
	) % 2);
}


void main() {
  vec3 normal = normalize(vn); // vn may have been interpolated so normalize it
  vec3 light_direction = normalize(light_position - v);
  float diffuse_factor = clamp(dot(normal, light_direction), 0.0f, 1.0f);
  //diffuse_factor = 0.0;

  vec3 view_direction = normalize(eye - v);
  vec3 reflect_direction = reflect(-light_direction, normal);
  float specular_factor = pow(clamp(dot(view_direction, reflect_direction), 0.0f, 1.0f), 32);
  //specular_factor = 0.0;

  float c =
    checkerboard(vt.x, vt.y, 1) * 0.4 +
    checkerboard(vt.x, vt.y, 1/8.0) * 0.3 +
    checkerboard(vt.x, vt.y, 1/64.0) * 0.2 +
    0.1;
  col = color_a + (color_d * vec4(vec3(c/2 + 0.3), 1) * diffuse_factor) + (color_s * specular_factor) + color_e;
}
