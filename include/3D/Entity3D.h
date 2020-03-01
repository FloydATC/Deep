#ifndef ENTITY3D_H
#define ENTITY3D_H


#include "Matrices.h"

/*
  This is a base class for all entities that exist in 3D space

  Entity3D      - direction, position, movement
    Camera3D    - perspective
    Prop3D      - instance of mesh

*/

class Entity3D
{
  public:
    // No public ctor
    virtual ~Entity3D();

    Matrix4 getMatrix();
    Matrix4 getRotationMatrix();
    Matrix4 getPositionMatrix();

    // Rotation
    void setDirection(float pitch, float yaw, float roll);
    void setDirection(Vector3 direction);
    Vector3 getDirection();
    void setPitch(float degrees);
    void addPitch(float degrees);
    float getPitch();
    void setYaw(float degrees);
    void addYaw(float degrees);
    float getYaw();
    void setRoll(float degrees);
    void addRoll(float degrees);
    float getRoll();

    // Position
    void setPosition(Vector3 position);
    Vector3 getPosition();


    // Movement relative to direction
    void strafeLeft(float distance);
    void strafeRight(float distance);
    void strafeUp(float distance);
    void strafeDown(float distance);
    void strafeForward(float distance);
    void strafeBackward(float distance);

    // Absolute movement
    void moveEast(float distance);
    void moveWest(float distance);
    void moveUp(float distance);
    void moveDown(float distance);
    void moveNorth(float distance);
    void moveSouth(float distance);


  protected:
    Entity3D();
    virtual void recalculate_matrix() = 0;

    Matrix4 matrix;
    Matrix4 rotation_matrix;
    Matrix4 position_matrix;
    bool need_recalc; // true if matrices need to be recalculated

    // Rotation
    float pitch;
    float yaw;
    float roll;

    // Position
    Vector3 position;

  private:


};

#endif // ENTITY3D_H
