#version 330 core
in vec4 v_color;
in vec2 v_uv;
uniform sampler2D texture;
out vec4 col;
void main() {
  if (v_color.a==0.0) {
    col = vec4(v_color.rgb, 1.0) * texture2D( texture, v_uv );
  } else {
    col = v_color;
  }
}
