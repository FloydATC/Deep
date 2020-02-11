#include "Scene3D.h"

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
    // std::cout << "Deleting " << obj.first << std::endl;
    delete obj.second; // first=key, second=value
  }
  std::cout << "Scene3D() destroyed" << std::endl;
}


Obj3D* Scene3D::getObj3D(const std::string filename)
{
  if (obj3d.count(filename) > 0) return obj3d.at(filename);
  Obj3D* obj = obj_loader.load(filename);
  obj3d.insert({filename, obj});
  return obj;
}

Prop3D* Scene3D::addProp(const std::string filename, Vector3 position)
{
  Prop3D* prop = new Prop3D();
  prop->setObject(getObj3D(filename));
  prop->setPosition(position);
  prop3d.push_back(prop);
  return prop;
}


Camera3D* Scene3D::camera()
{
  return &this->cam;
}

void Scene3D::render()
{

}

