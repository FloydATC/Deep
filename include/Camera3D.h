#ifndef CAMERA3D_H
#define CAMERA3D_H

#include "Matrices.h"

class Camera3D
{
  public:
    Camera3D();
    ~Camera3D();

    Matrix4 matrix();

    void setDimensions(int width, int height);
    void setAspect(float width_by_height);
    float getAspect();
    int getWidth();
    int getHeight();
    void setFOV(float degrees);
    void addFOV(float degrees);
    float getFOV();
    void setPitch(float degrees);
    void addPitch(float degrees);
    float getPitch();
    void setYaw(float degrees);
    void addYaw(float degrees);
    float getYaw();
    void setPosition(Vector3 position);

  protected:

  private:
    bool need_recalc;
    void recalculate();

    float fov;
    float aspect;
    int width;
    int height;
    float pitch;
    float yaw;
    Vector3 pos;
    Matrix4 mat;

};

#endif // CAMERA3D_H
