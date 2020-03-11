#version 330 core

// Render display screen in 3D scene

in vec2 vt;
in vec3 vn;

uniform vec4 color_a;
uniform vec4 color_d;
uniform vec4 color_s;
uniform vec4 color_e;

uniform vec2 decal_position;
uniform sampler2D decal_texture;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_bump;

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

float checkerboard(float x, float y, float scale) {
	return float((
		int(floor(x / scale)) +
		int(floor(y / scale))
	) % 2);
}


void main() {

  if (is_debug == 0 && use_texture > 0) {
    // Simulate convergence error and shadowing
    float r = max(separation(0, vt, +0.0008), shadow(0, vt, -0.0008));
    float b = max(separation(2, vt, +0.0000), shadow(2, vt, -0.0008));
    float g = max(separation(1, vt, -0.001), shadow(1, vt, -0.0008));

    col = color_d * vec4( r, g, b, 1.0 );
  }
}
