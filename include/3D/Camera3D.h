#ifndef CAMERA3D_H
#define CAMERA3D_H

#include "Matrices.h"
#include "3D/Entity3D.h"

class Camera3D : public Entity3D
{
  public:
    Camera3D();
    ~Camera3D();

    Matrix4 getPerspectiveMatrix();
    Matrix4 getViewMatrix();
    Matrix4 getRotationMatrix(); // Camera rotation must be applied in reverse order
    Matrix4 getPositionMatrix(); // Camera position must be applied in opposite direction

    Vector3 getDirection();
    void setDimensions(int width, int height);
    inline Vector2 getDimensions() { return Vector2(this->width, this->height); }
    void setAspect(float width_by_height);
    float getAspect();
    int getWidth();
    int getHeight();
    void setFOV(float degrees);
    void addFOV(float degrees);
    float getFOV();
    float getNear();
    float getFar();



  protected:

  private:
    void recalculate_matrix();

    float fov;
    float aspect;
    float clip_near = 0.1;
    float clip_far = 100.0;
    int width;
    int height;


    Matrix4 perspective_matrix;
    Matrix4 view_matrix;


};

#endif // CAMERA3D_H
