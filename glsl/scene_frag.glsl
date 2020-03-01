#version 330 core
in vec3 v_normal;
in vec4 v_color;
in vec2 v_uv;
uniform sampler2D texture;
uniform int is_debug;
uniform int use_texture;

out vec4 col;

float separation(int component, vec2 tex, float offset) {
  return texture2D( texture, vec2(tex.x+offset, tex.y))[component];
}

float shadow(int component, vec2 tex, float offset) {
  float pix = texture2D( texture, vec2(tex.x, tex.y))[component];
  float sha = texture2D( texture, vec2(tex.x+offset, tex.y))[component] * 0.3;
  return max(pix, sha);
}

float checkerboard(float x, float y, float scale) {
	return float((
		int(floor(x / scale)) +
		int(floor(y / scale))
	) % 2);
}

void main() {

  if (is_debug > 0) {
    if (use_texture > 0) {
      float c =
        checkerboard(v_uv.x, v_uv.y, 1) * 0.3 +
        checkerboard(v_uv.x, v_uv.y, 0.1) * 0.2 +
        checkerboard(v_uv.x, v_uv.y, 0.01) * 0.1 +
        0.1;
	    col = vec4(vec3(c/2.0 + 0.3), 1);
    } else {
      col = vec4(1.0, 0.0, 0.0, 1.0) + vec4(v_normal.x, v_normal.y, v_normal.z, 1.0); // Prevent optimization
    }
  } else {
    if (use_texture > 0) {
      // Simulate convergence error and shadowing
      float r = max(separation(0, v_uv, +0.0008), shadow(0, v_uv, -0.0008));
      float b = max(separation(2, v_uv, +0.0000), shadow(2, v_uv, -0.0008));
      float g = max(separation(1, v_uv, -0.001), shadow(1, v_uv, -0.0008));

      // For debugging: perfect texture sampling
      //float r = texture2D( texture, vec2(v_uv.x, v_uv.y) )[0];
      //float b = texture2D( texture, vec2(v_uv.x, v_uv.y) )[2];
      //float g = texture2D( texture, vec2(v_uv.x, v_uv.y) )[1];

      col = v_color * vec4( r, g, b, 1.0 );
      //col = vec4(1.0, 1.0, 1.0, 1.0) * texture2D( texture, v_uv );
    } else {
      col = v_color;
    }
  }
}
