#version 330 core

// Render shadow volume in 3D scene

layout(location = 0) in vec3 attr_v;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
  mat4 m = projection * view * model;

  gl_Position = m * vec4(attr_v, 1.0);
}
