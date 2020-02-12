#include "Scene3D.h"

#include "GFX.h"

Scene3D::Scene3D()
{
  //ctor
  this->cam = Camera3D();
  std::cout << "Scene3D() created" << std::endl;
}

Scene3D::~Scene3D()
{
  //dtor

  // Destroy all Prop3D objects in std::vector
  for (const auto& prop : prop3d) {
    delete prop;
  }

  // Destroy all Obj3D objects in std::unordered_map
  for( const auto& obj : obj3d ) {
    delete obj.second; // first=key, second=value
  }

  // Destroy all Obj3D objects in std::unordered_map
  for( const auto& shader : shaderProgram ) {
    delete shader.second; // first=key, second=value
  }
  std::cout << "Scene3D() destroyed" << std::endl;
}


void Scene3D::render()
{
  glViewport(0, 0, this->cam.getWidth(), this->cam.getHeight());

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  for (const auto& prop : prop3d) {
    prop->render(this->cam.matrix());
  }

}


ShaderProgram* Scene3D::getShader(const std::string vs_filename, const std::string fs_filename)
{
  if (shaderProgram.count(vs_filename) > 0) return shaderProgram.at(vs_filename+fs_filename);
  ShaderProgram* shader = new ShaderProgram(vs_filename, fs_filename);
  shaderProgram.insert({vs_filename+fs_filename, shader});
  return shader;
}

Obj3D* Scene3D::getObj3D(const std::string filename)
{
  if (obj3d.count(filename) > 0) return obj3d.at(filename);
  Obj3D* obj = obj_loader.load(filename);
  obj3d.insert({filename, obj});
  return obj;
}

Prop3D* Scene3D::addProp(Obj3D* object)
{
  Prop3D* prop = new Prop3D();
  prop->setObject(object);
  prop3d.push_back(prop);
  return prop;
}

Camera3D* Scene3D::camera()
{
  return &this->cam;
}

