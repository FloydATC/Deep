#include "3D/Scene3D.h"

#include "GFX.h"

//#define DEBUG_TRACE_SCENE

Scene3D::Scene3D()
{
  //ctor
  this->cam = new Camera3D();
  this->shadow_shader = nullptr;
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

  //int stencil_bits = 0;
	//glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
	//glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
	//SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_bits);
	//std::cout << "stencil bits=" << stencil_bits << std::endl;

  glViewport(0, 0, this->cam->getWidth(), this->cam->getHeight());

  glClearColor(0.04f, 0.04f, 0.05f, 1.0f);
  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_CLAMP);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_STENCIL_TEST);

  // Ambient color pass: Render 3D models
  for (const auto& prop : prop3d) {
    prop.second->renderAmbient(this->cam);
  }

  glDepthFunc(GL_LEQUAL);

  // Calculate and draw shadow volumes for each light and each mesh

  for (const auto& light_pair : light3d) {
    Light3D* light = light_pair.second;
    if (light->isEnabled() == false) continue;

    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x0, 0xFF);
    glStencilMask(0xFF);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);



    for (const auto& prop_pair : prop3d) {
      Prop3D* prop = prop_pair.second;
      if (prop->castsShadow() == false) continue;

      prop->generateShadowVolumes(light);

      glDepthFunc(GL_LESS);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);

      prop->renderShadowVolumes(this->cam, this->shadow_shader);

      glCullFace(GL_FRONT);
      prop->renderShadowVolumes(this->cam, this->shadow_shader);
      glCullFace(GL_BACK);

      prop->destroyShadowVolumes();
    }


    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0xFF);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Diffuse + Specular color pass: Render 3D models
    for (const auto& prop : prop3d) {
      prop.second->renderLight(this->cam, light);
    }

  }

  /*
void perspectiveProjectionMatrix(double left,
                                 double right,
                                 double bottom,
                                 double top,
                                 double nearval,
                                 double farval)
{
  double x, y, a, b, c, d;
  x = (2.0 * nearval) / (right - left);
  y = (2.0 * nearval) / (top - bottom);
  a = (right + left) / (right - left);
  b = (top + bottom) / (top - bottom);

  if ((float)farval >= (float)inf) {
    // Infinite view frustum
    c = -1.0;
    d = -2.0 * nearval;
  } else {
    c = -(farval + nearval) / (farval - nearval);
    d = -(2.0 * farval * nearval) / (farval - nearval);
  }
  double m[] = {
    x, 0, 0, 0,
    0, y, 0, 0,
    a, b, c, -1,
    0, 0, d, 0
  };
  glLoadMatrixd(m);
}
*/




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
  return (int)prop3d.size();
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
  return (int)light3d.size();
}


Camera3D* Scene3D::getCamera()
{
  return this->cam;
}


void Scene3D::setStandardShader(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::setStandardShader() shader=" << shader << std::endl;
#endif
  for (const auto& prop : this->prop3d) prop.second->setShader(shader);
}


void Scene3D::setShadowShader(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_SCENE
  std::cout << "Scene3D" << this << "::setShadowShader() shader=" << shader << std::endl;
#endif
  this->shadow_shader = shader;
}


