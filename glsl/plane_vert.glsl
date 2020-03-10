#version 330 core
layout(location = 0) in vec3 attr_v;
layout(location = 1) in vec2 attr_vt;
layout(location = 2) in vec3 attr_vn;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 vt;
out vec3 vn;

void main() {
  mat4 m = projection * view * model;

  vt = attr_vt;
  vn = (model * vec4(attr_vn, 1.0)).xyz;
  gl_Position = m * vec4(attr_v, 1.0);
}
