#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
uniform mat4 scene;
uniform mat4 model;
uniform vec4 color;
uniform int is_debug;
out vec3 v_normal;
out vec4 v_color;
out vec2 v_uv;
void main() {
  v_normal = normal;
  v_uv = uv;
  v_color = color;
  gl_Position = scene * model * vec4(vertex, 1.0);
}
