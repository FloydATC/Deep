#version 330 core
layout(location = 0) in vec3 attr_v;
layout(location = 1) in vec2 attr_vt;
layout(location = 2) in vec3 attr_vn;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 v;
out vec2 vt;
out vec3 vn;
out vec3 eye;


void main() {
  mat4 m = projection * view * model;

  vt = attr_vt;
  vn = mat3(model) * attr_vn; // For lighting
  v =  vec4(model * vec4(attr_v, 1.0)).xyz; // For lighting

  mat4 iv = inverse(view);
  eye = vec3(iv[3][0], iv[3][1],iv[3][2]);

  gl_Position = m * vec4(attr_v, 1.0);
}
