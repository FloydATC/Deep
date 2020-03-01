#version 330 core
in vec3 v_normal;
in vec4 v_color;
in vec2 v_uv;
uniform sampler2D texture;
uniform int is_debug;
uniform int use_texture;
out vec4 col;
void main() {
  if (is_debug > 0) {
    col = vec4(1.0, 0.0, 0.0, 1.0) + vec4(v_normal.x, v_normal.y, v_normal.z, 1.0); // Prevent optimization
  } else {
    if (use_texture > 0) {
      col = v_color * texture2D( texture, v_uv );
    } else {
      col = v_color;
    }
  }
}
