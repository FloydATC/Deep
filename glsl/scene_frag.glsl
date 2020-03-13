#version 330 core
in vec3 v;
in vec2 vt;
in vec3 vn;
in vec3 eye;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_bump;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

uniform int is_debug;
uniform int use_texture;

vec3 light_position = vec3( -0.10, 1.00, 0.5);

out vec4 col;



void main() {
  vec3 normal = normalize(vn); // vn may have been interpolated so normalize it
  vec3 light_direction = normalize(light_position - v);
  float diffuse_factor = clamp(dot(normal, light_direction), 0.0f, 1.0f);

  vec3 view_direction = normalize(eye - v);
  vec3 reflect_direction = reflect(-light_direction, normal);
  float specular_factor = pow(clamp(dot(view_direction, reflect_direction), 0.0f, 1.0f), 32);

  if (is_debug > 0) {
    col = vec4(1.0, 0.0, 0.0, 1.0);
  } else {
    if (use_texture > 0) {
      col = color_a + (color_d * texture2D( texture_diffuse, vt ) * diffuse_factor) + (color_s * specular_factor);
    } else {
      col = color_d;
    }
  }
}
