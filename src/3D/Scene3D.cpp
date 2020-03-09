#include "3D/Scene3D.h"

#include "GFX.h"

//#define DEBUG_TRACE_SCENE

Scene3D::Scene3D()
{
  //ctor
  this->cam = new Camera3D();
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D() created" << std::endl;
#endif
}

Scene3D::~Scene3D()
{
  //dtor
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << " destruction" << std::endl;
#endif

  // Destroy all Prop3D objects in std::vector
  for (const auto& prop : prop3d) {
    delete prop;
  }

  // Destroy all Obj3D objects in std::unordered_map
  for( const auto& obj : obj3d ) {
    delete obj.second; // first=key, second=value
  }

  // Destroy all Texture objects in std::unordered_map
  for( const auto& tex : texture ) {
    delete tex.second; // first=key, second=value
  }

  // Destroy all ShaderProgram objects in std::unordered_map
  for( const auto& shader : shaderProgram ) {
    delete shader.second; // first=key, second=value
  }

  delete this->cam;
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << " destruction complete" << std::endl;
#endif
}


void Scene3D::render()
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::render() begin" << std::endl;
#endif
  glViewport(0, 0, this->cam->getWidth(), this->cam->getHeight());

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // Render 3D models
  for (const auto& prop : prop3d) {
    prop->render(this->cam);
  }

#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::render() completed" << std::endl;
#endif
}


void Scene3D::setDimensions(int width, int height)
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::setDimensions() width=" << width << " height=" << height << std::endl;
#endif
  this->cam->setDimensions(width, height);
}


int Scene3D::getWidth()
{
  return this->cam->getWidth();
}


int Scene3D::getHeight()
{
  return this->cam->getHeight();
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


Prop3D* Scene3D::addProp(Mesh3D* mesh)
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::addProp() mesh=" << mesh << " name=" << mesh->getName() << std::endl;
#endif
  Prop3D* prop = new Prop3D();
  prop->setMesh(mesh);
  prop3d.push_back(prop);
  return prop;
}


Prop3D* Scene3D::getProp(int index)
{
  if (index < 0 || index > ((int)prop3d.size())-1) return nullptr;
  return prop3d[index];
}


int Scene3D::getPropCount()
{
  return prop3d.size();
}


Camera3D* Scene3D::getCamera()
{
  return this->cam;
}


void Scene3D::setShader(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::setShader() shader=" << shader << std::endl;
#endif
  for (const auto& prop : this->prop3d) prop->setShader(shader);
}

