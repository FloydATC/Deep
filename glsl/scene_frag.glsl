#version 330 core
in vec4 v_color;
in vec2 v_uv;
uniform sampler2D texture;
uniform int is_debug;
out vec4 col;

float separation(int component, vec2 tex, float offset) {
  return texture2D( texture, vec2(tex.x+offset, tex.y))[component];
}

float shadow(int component, vec2 tex, float offset) {
  float pix = texture2D( texture, vec2(tex.x, tex.y))[component];
  float sha = texture2D( texture, vec2(tex.x+offset, tex.y))[component] * 0.3;
  return max(pix, sha);
}

void main() {

  if (is_debug == 1) {
    col = vec4(1.0, 0.0, 0.0, 1.0);
  } else {

  // Simulate convergence error and shadowing
  float r = max(separation(0, v_uv, +0.0008), shadow(0, v_uv, -0.0008));
  float b = max(separation(2, v_uv, +0.0000), shadow(2, v_uv, -0.0008));
  float g = max(separation(1, v_uv, -0.001), shadow(1, v_uv, -0.0008));

  // For debugging: perfect texture sampling
//  float r = texture2D( texture, vec2(v_uv.x, v_uv.y) )[0];
//  float b = texture2D( texture, vec2(v_uv.x, v_uv.y) )[2];
//  float g = texture2D( texture, vec2(v_uv.x, v_uv.y) )[1];

  col = vec4(1.0, 1.0, 1.0, 1.0) * vec4( r, g, b, 1.0 );
  //col = vec4(1.0, 1.0, 1.0, 1.0) * texture2D( texture, v_uv );

  }
}
