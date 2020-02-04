#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
uniform mat4 scene;
uniform mat4 model;
out vec4 v_color;
out vec2 v_uv;
void main() {
  v_uv = uv;
  gl_Position = scene * model * vec4(vertex, 1.0);
}
