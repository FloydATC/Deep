#include "Prop3D.h"


Prop3D::Prop3D()
{
  //ctor
  this->object = nullptr;
  this->shader = nullptr;
  this->pos = Vector3(0.0, 0.0, 0.0);
  this->dir = Vector3(0.0, 0.0, 0.0);
  this->scale = Vector3(1.0, 1.0, 1.0);
  this->need_recalc = true;
}

Prop3D::~Prop3D()
{
  //dtor
}


void Prop3D::render(Matrix4 scene_matrix) {
  if (this->need_recalc) this->recalculate();

  glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
  glBindTexture(GL_TEXTURE_2D, this->texture);

  glUseProgram(this->shader->id());
  this->object->set_shader_v(this->shader_v);
  this->object->set_shader_vt(this->shader_vt);
  this->object->set_shader_vn(this->shader_vn);

  // FIXME: get rid of hardcoded shader uniform names
  GLuint uniform_scene = glGetUniformLocation(this->shader->id(), "scene");
  GLuint uniform_model = glGetUniformLocation(this->shader->id(), "model");
  glUniformMatrix4fv(uniform_scene, 1, GL_FALSE, scene_matrix.get());
  glUniformMatrix4fv(uniform_model, 1, GL_FALSE, this->mat.get());

  for (int i=0; i<this->object->subobjects; i++) {
    this->object->render(i);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glDisableVertexAttribArray(0);
}

void Prop3D::recalculate() {
  std::cout << "Prop3D::recalculate() begin" << std::endl;
  this->mat = Matrix4().translate(this->pos); // Relative position
  this->mat *= Matrix4().rotate(this->dir.z, Vector3(0.0f, 0.0f, 1.0f)); // Rotation angle and axis
  this->mat *= Matrix4().rotate(this->dir.y, Vector3(0.0f, 1.0f, 0.0f)); // Rotation angle and axis
  this->mat *= Matrix4().rotate(this->dir.x, Vector3(1.0f, 0.0f, 0.0f)); // Rotation angle and axis
  this->mat *= Matrix4().scale(this->scale.x, this->scale.y, this->scale.z);
  std::cout << "Prop3D::recalculate() done" << std::endl;
  this->need_recalc = false;
}


Obj3D* Prop3D::Object()
{
  return this->object;
}


void Prop3D::setObject(Obj3D* object)
{
  this->object = object;
}

void Prop3D::setScale(float scale)
{
  this->scale = Vector3(scale, scale, scale);
  this->need_recalc = true;
}

void Prop3D::setScale(Vector3 scale)
{
  this->scale = scale;
  this->need_recalc = true;
}

void Prop3D::setPosition(Vector3 position)
{
  this->pos = position;
  this->need_recalc = true;
}

void Prop3D::setDirection(Vector3 direction)
{
  this->dir = direction;
  this->need_recalc = true;
}

void Prop3D::setTexture(GLuint texture)
{
  this->texture = texture;
}

void Prop3D::setShader(ShaderProgram* shader, const std::string v, const std::string vt, const std::string vn)
{
  this->shader = shader;
  this->shader_v = glGetAttribLocation(shader->id(), v.c_str());
  this->shader_vt = glGetAttribLocation(shader->id(), vt.c_str());
  this->shader_vn = glGetAttribLocation(shader->id(), vn.c_str());

  // TODO: Remove when we render via ::render() function
  this->object->set_shader_v(this->shader_v);
  this->object->set_shader_vt(this->shader_vt);
  this->object->set_shader_vn(this->shader_vn);
}
