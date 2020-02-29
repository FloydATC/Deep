#ifndef CAMERA3D_H
#define CAMERA3D_H

#include "Matrices.h"

class Camera3D
{
  public:
    Camera3D();
    ~Camera3D();

    Matrix4 getMatrix();

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
    Vector3 getPosition();
    void setPitch(float degrees);
    void addPitch(float degrees);
    float getPitch();
    void setYaw(float degrees);
    void addYaw(float degrees);
    float getYaw();
    void setPosition(Vector3 position);
    Matrix4 getRotationMatrix();
    void strafeLeft(float distance);
    void strafeRight(float distance);
    void strafeUp(float distance);
    void strafeDown(float distance);
    void strafeForward(float distance);
    void strafeBackward(float distance);

  protected:

  private:
    bool need_recalc;
    void recalculate();

    float fov;
    float aspect;
    float clip_near = 0.1;
    float clip_far = 100.0;
    int width;
    int height;
    float pitch;
    float yaw;
    Vector3 pos;
    Matrix4 mat;

};

#endif // CAMERA3D_H
