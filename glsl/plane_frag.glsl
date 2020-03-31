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

uniform vec4 light_position;
uniform vec4 light_color;

out vec4 col;

float checkerboard(float x, float y, float scale) {
	return float((
		int(floor(x / scale)) +
		int(floor(y / scale))
	) % 2);
}


void main() {
  vec3 normal = normalize(vn); // vn may have been interpolated so normalize it
  vec3 light_direction = normalize(light_position.xyz - v);
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
  if (color_a.a > 0) {
    // Ambient pass
    col = color_a * vec4(vec3(c/2 + 0.4), 1) + color_e;
  } else {
    // Light pass
    vec4 diffuse = color_d * vec4(vec3(c/2 + 0.4), 1) * light_color * diffuse_factor;
    //diffuse.a = diffuse_factor;
    vec4 specular = color_s * vec4(vec3(c/2 + 0.4), 1) * light_color * specular_factor;
    //specular.a = specular_factor;
    col = diffuse + specular;
  }
  col.a = 1.0;
}
