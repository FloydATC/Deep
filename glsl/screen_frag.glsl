#version 330 core

// Render display screen in 3D scene

in vec2 vt;
in vec3 vn;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

uniform vec2 position_decal;
uniform sampler2D texture_decal;

uniform sampler2D texture_diffuse;

uniform int is_debug;
uniform int use_texture;

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

  if (is_debug == 0 && use_texture > 0) {
    // Simulate convergence error and shadowing
    float r = max(separation(0, vt, +0.0008), shadow(0, vt, -0.0008));
    float b = max(separation(2, vt, +0.0000), shadow(2, vt, -0.0008));
    float g = max(separation(1, vt, -0.001), shadow(1, vt, -0.0008));

    col = color_d * vec4( r, g, b, 1.0 );
    vec4 cursor = texture2D( texture_decal, vec2( ((vt.x - position_decal.x*1.2 - 0.5)*60.0), (vt.y - position_decal.y*1.42 - 0.47)*30.0));
    if (cursor.a > 0.5) col = cursor;
  }
}
