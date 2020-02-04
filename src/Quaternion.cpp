// Quaternion.cpp: implementation of the Quaternion class.
//
//////////////////////////////////////////////////////////////////////

// http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation


#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"
#include <math.h> // atan2(), sqrt()


/*
 * Quaternion storage and calculation class
 *
 * Copyright 2006 (c) Andreas Lund <floyd@atc.no> All Rights Reserved
 */

class Quaternion
{

public:
  float x;
  float y;
  float z;
  float w;

  //Quaternion();
  //Quaternion( const Quaternion & );
  //Quaternion( const float, const Vector & );
  //Quaternion( const float, const float, const float, const float );

	//virtual ~Quaternion();

  //void normalize();
  //void conjugate();
  //void invert();

  //void getFloatV( float (&)[16] ) const;




  //////////////////////////////////////////////////////////////////////
  // Construction/Destruction
  //////////////////////////////////////////////////////////////////////

  // Constructor: uninitialized
  Quaternion() : x( 1 ), y( 0 ), z( 0 ), w( 0 ) {}
  // Constructor: copy
  Quaternion( const Quaternion & );
  // Constructor: initialized
  Quaternion( const float angle, const Vector &v )
  {
    float theta = angle;
    theta *= 0.5f;
    float sintheta = (float) sin(theta);
    x = v.x * sintheta;
    y = v.y * sintheta;
    z = v.z * sintheta;
    w = (float) cos(theta);
  }
  // Constructor: initialized
  Quaternion( const float _w, const float _x, const float _y, const float _z) : x( _x ), y( _y ), z( _z ), w( _w ) {}

  // Destructor
  ~Quaternion()
  {

  }


  //////////////////////////////////////////////////////////////////////
  // Class methods
  //////////////////////////////////////////////////////////////////////


  /* Normalising a quaternion works similar to a vector
  */
  void normalize()
  {
    float magnitude = (float) sqrt(w * w + x * x + y * y + z * z);
    w /= magnitude;
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
  }

  /* The conjugate of a quaternion is the same as the inverse, as long as the quaternion is unit-length
  */
  void conjugate()
  {
    x = -x;
    y = -y;
    z = -z;
  }

  /* For non-unit quaternions, here is how to get the inverse
  */
  void invert()
  {
    conjugate();
    const float norm = (x * x) + (y * y) + (z * z) + (w * w);

    if (norm == 0.0f)
      return; // Invalid quaternion

    const float inv_norm = 1 / norm;
    x *= inv_norm;
    y *= inv_norm;
    z *= inv_norm;
    w *= inv_norm;

  }

  void getFloatV(float (&m)[16]) const {
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;


    // This calculation would be a lot more complicated for non-unit length quaternions
    // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
    //   OpenGL
    m[ 0] = 1.0f - 2.0f * (y2 + z2);
    m[ 1] = 2.0f * (xy - wz);
    m[ 2] = 2.0f * (xz + wy);
    m[ 3] = 0.0f;

    m[ 4] = 2.0f * (xy + wz);
    m[ 5] = 1.0f - 2.0f * (x2 + z2);
    m[ 6] = 2.0f * (yz - wx);
    m[ 7] = 0.0f;

    m[ 8] = 2.0f * (xz - wy);
    m[ 9] = 2.0f * (yz + wx);
    m[10] = 1.0f - 2.0f * (x2 + y2);
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;

  //  return m;

  }




  //////////////////////////////////////////////////////////////////////
  // Class functions
  //////////////////////////////////////////////////////////////////////


  /* Multiply a quaternion by another quaternion instance */
/*
  Quaternion operator * ( const Quaternion &q1, const Quaternion &q2 )
  {
    return Quaternion (
      q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
      q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
      q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
      q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
    );
  }
*/
  /* Multiply this quaternion by another quaternion instance */
  void operator *= ( const Quaternion &q2 )
  {
    Quaternion q1;
    q1.x = x;
    q1.y = y;
    q1.z = z;
    q1.w = w;

    x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  }

  /* Make this quaternion identical to another instance */
  void operator = ( const Quaternion &q2 )
  {
    x = q2.x;
    y = q2.y;
    z = q2.z;
    w = q2.w;
  }

};
