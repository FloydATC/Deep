#ifndef OBJ3D_H
#define OBJ3D_H

#include <vector>
#include <string>

#include "GFX.h"
#include <SDL.h>

#include "3D/Box3D.h"
#include "3D/Mesh3D.h"
#include "ShaderProgram.h"

/*

  An Obj3D encapsulates the contents of a Wavefront .OBJ file
  represented by one or more vertex array buffers.

  Reflecting the layout of such files, the actual mesh data is contained within
  SubObject3D objects, or other types of Mesh3D objects.

  An Obj3D can also be decorated with planes, rays and other parts.

*/


class Obj3D : public Mesh3D
{
  public:
    Obj3D();
    ~Obj3D();

    void render(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader);
    void setShader(ShaderProgram* shader);

    void addPart(Mesh3D* mesh);
    Mesh3D* getPart(int index);
    int numParts();
    void setTexture(GLuint texture);

    void setDecalTexture(GLuint texture);
    void setDecalPosition(Vector2 position);

  protected:

    std::vector<Mesh3D*> parts;

  private:

};

#endif // OBJ3D_H
