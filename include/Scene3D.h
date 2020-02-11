#ifndef SCENE3D_H
#define SCENE3D_H

#include <unordered_map>
#include <string>

#include "Camera3D.h"
#include "Obj3D.h"
#include "Obj3DLoader.h"

#include "Prop3D.h"

class Scene3D
{
  public:
    Scene3D();
    ~Scene3D();

    void render();
    Obj3D* getObj3D(const std::string filename);
    Prop3D* addProp(const std::string filename, Vector3 position);
    Camera3D* camera();

  protected:

  private:
    Obj3DLoader obj_loader = Obj3DLoader();

    std::unordered_map<std::string, Obj3D*> obj3d;
    std::vector<Prop3D*> prop3d;
    Camera3D cam;
};

#endif // SCENE3D_H
