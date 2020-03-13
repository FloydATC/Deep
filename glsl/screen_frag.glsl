#version 330 core

// Render display screen in 3D scene

in vec3 v;
in vec2 vt;
in vec3 vn;
in vec3 eye;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

uniform vec2 position_decal;
uniform sampler2D texture_decal;

uniform sampler2D texture_diffuse;

uniform int is_debug;
uniform int use_texture;

vec3 light_position = vec3( -0.20, 1.00, 1.50);

out vec4 col;

float separation(int component, vec2 tex, float offset) {
  return texture2D( texture_diffuse, vec2(tex.x+offset, tex.y))[component];
}

float shadow(int component, vec2 tex, float offset) {
  float pix = texture2D( texture_diffuse, vec2(tex.x, tex.y))[component];
  float sha = texture2D( texture_diffuse, vec2(tex.x+offset, tex.y))[component] * 0.3;
  return max(pix, sha);
}


void main() {
  vec3 normal = normalize(vn); // vn may have been interpolated so normalize it
  vec3 light_direction = normalize(light_position - v);
  float diffuse_factor = clamp(dot(normal, light_direction), 0.0f, 1.0f);

  vec3 view_direction = normalize(eye - v);
  vec3 reflect_direction = reflect(-light_direction, normal);
  float specular_factor = pow(clamp(dot(view_direction, reflect_direction), 0.0f, 1.0f), 128);

  if (is_debug == 0 && use_texture > 0) {
    // Simulate convergence error and shadowing
    float r = max(separation(0, vt, +0.0008), shadow(0, vt, -0.0008));
    float b = max(separation(2, vt, +0.0000), shadow(2, vt, -0.0008));
    float g = max(separation(1, vt, -0.001), shadow(1, vt, -0.0008));

    col = color_a + (color_d * diffuse_factor) + (color_s * specular_factor) + vec4( r, g, b, 1.0 );
    vec4 cursor = texture2D( texture_decal, vec2( ((vt.x - position_decal.x*1.2 - 0.5)*60.0), (vt.y - position_decal.y*1.42 - 0.47)*30.0));
    if (cursor.a > 0.5) col = cursor;
  }
}
