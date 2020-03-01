#version 330 core
layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
uniform mat4 ortho;
uniform vec4 color;
out vec3 v_normal;
out vec4 v_color;
out vec2 v_uv;
void main() {
  v_normal = normal;
  v_color = color;
  v_uv = uv;
  gl_Position = ortho * vec4(vertex, 0.0, 1.0);
  gl_PointSize = 1.0;
}
