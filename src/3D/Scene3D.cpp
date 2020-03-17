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

  // Destroy all Prop3D objects in std::unordered_map
  for( const auto& prop : prop3d ) {
    delete prop.second; // first=key, second=value
  }

  // Destroy all Light3D objects in std::unordered_map
  for( const auto& light : light3d ) {
    delete light.second; // first=key, second=value
  }

  // Destroy all Material objects in std::vector
  for (const auto& material : materials) {
    delete material;
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

  glClearColor(0.04f, 0.04f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // Ambient color pass: Render 3D models
  for (const auto& prop : prop3d) {
    prop.second->render(this->cam);
  }

  // Calculate and draw shadow volumes for each light and each mesh
  for (const auto& light_pair : light3d) {
    Light3D* light = light_pair.second;
    if (light->isEnabled() == false) continue;


    for (const auto& prop_pair : prop3d) {
      Prop3D* prop = prop_pair.second;
      if (prop->castsShadow() == false) continue;

      prop->generateShadowVolumes(light);

      prop->renderShadowVolumes(GL_FRONT);

    }


    for (const auto& prop_pair : prop3d) {
      Prop3D* prop = prop_pair.second;
      if (prop->castsShadow() == false) continue;

      prop->renderShadowVolumes(GL_BACK);

      prop->destroyShadowVolumes();
    }
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


Material* Scene3D::getMaterial()
{
  Material* material = new Material();
  materials.push_back(material);
  return material;
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


Texture* Scene3D::getTexture(const std::string filename)
{
  if (texture.count(filename) > 0) return texture.at(filename);
  Texture* tex = new Texture();
  tex->load(filename);
  texture.insert({filename, tex});
  return tex;
}


Prop3D* Scene3D::addProp(std::string name, Mesh3D* mesh)
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::addProp() mesh=" << mesh << " name=" << mesh->getName() << std::endl;
#endif
  Prop3D* prop = new Prop3D();
  prop->setMesh(mesh);
  prop3d.insert({ name, prop });
  return prop;
}


Prop3D* Scene3D::getPropByName(std::string name)
{
  if (prop3d.count(name) == 0) return nullptr;
  return prop3d.at(name);
}


int Scene3D::getPropCount()
{
  return prop3d.size();
}


void Scene3D::addLight(std::string name, Light3D* light)
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::addLight() light=" << light << " name=" << name << std::endl;
#endif
  light3d.insert({ name, light });
  return;
}


Light3D* Scene3D::getLightByName(std::string name)
{
  if (light3d.count(name) == 0) return nullptr;
  return light3d.at(name);
}


int Scene3D::getLightCount()
{
  return light3d.size();
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
  for (const auto& prop : this->prop3d) prop.second->setShader(shader);
}

