#ifndef SCENE3D_H
#define SCENE3D_H

#include <unordered_map>
#include <string>

#include "3D/Camera3D.h"
#include "3D/Obj3D.h"
#include "3D/Obj3DLoader.h"
#include "3D/Prop3D.h"
#include "Matrices.h"
#include "ShaderProgram.h"
#include "Texture.h"

/*

  An instance of this class can contain 3D entities and render them
  The class also acts as a factory to load
    Obj3D mesh objects (via Obj3DLoader)
    ShaderProgram container objects

*/

class Scene3D
{
  public:
    Scene3D();
    ~Scene3D();

    void render();
    void setDimensions(int width, int height);
    int getWidth();
    int getHeight();
    ShaderProgram* getShader(const std::string vs_filename, const std::string fs_filename);
    void setShader(ShaderProgram* shader);

    Obj3D* getObj3D(const std::string filename);

    Texture* getTexture(const std::string filename);

    Prop3D* addProp(Mesh3D* mesh);
    Prop3D* getProp(int index);
    int getPropCount();
    Camera3D* getCamera();

  protected:

  private:
    Obj3DLoader obj_loader = Obj3DLoader();

    std::unordered_map<std::string, ShaderProgram*> shaderProgram;
    std::unordered_map<std::string, Obj3D*> obj3d;
    std::unordered_map<std::string, Texture*> texture;
    std::vector<Prop3D*> prop3d;
    Camera3D* cam;
};

#endif // SCENE3D_H
