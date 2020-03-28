
#ifndef MATH3D_H
#define MATH3D_H

#include "Vectors.h"

#define EPSILON 0.00001


bool point_inside_convex_polygon(Vector2 point, std::vector<Vector3> polygon)
{
  // Return true if point is inside the convex polygon
  // Note: The polygon points must be specified in clockwise order
  if (polygon.size() < 3) return false; // less than a triangle

  // Calculate the dot product each of the polygons
  // described by each line of the polygon and the point;
  // If one is negative, the point is outside and we return false.

  // We need 3-component vectors to calculate cross products but must zero the Z components
  Vector3 p0;
  Vector3 p1;
  Vector3 p2 = Vector3(point.x, point.y, 0.0);
  int sides = polygon.size();

  int last = sides-1;
  for (int i=0; i<sides; i++) {
    p0 = Vector3(polygon[i].x, polygon[i].y, 0.0);
    if (i == last) {
      p1 = Vector3(polygon[0].x, polygon[0].y, 0.0);
    } else {
      p1 = Vector3(polygon[i+1].x, polygon[i+1].y, 0.0);
    }
    if ((p2 - p0).cross(p0 - p1).z < 0) return false;
  }

  return true;
}


Vector3 projected_vector(Vector3 world_point, Vector2 display, Matrix4 projection, Matrix4 view, Matrix4 model)
{
  // This function is used to project (transform) a 3D point in worldspace
  // to display coordinates

  // We need a Vector4 for the projection since the matrices are 4x4 and use perspective
  Vector4 v4 = projection * view * model * Vector4(world_point.x, world_point.y, world_point.z, 1.0);

  // Return 2D display coordinates + depth component
  float w2 = display.x / 2;
  float h2 = display.y / 2;
  return Vector3(w2+(v4.x*w2/v4.w), h2+(v4.y*-h2/v4.w), v4.z/v4.w); // flip Y so 0,0 is upper left corner
}


Vector3 unprojected_vector(Vector2 pixel, Vector2 display, Matrix4 projection, Matrix4 view)
{
  // http://antongerdelan.net/opengl/raycasting.html
  float x = (2.0f * pixel.x) / display.x - 1.0f;
  float y = 1.0f - (2.0f * pixel.y) / display.y;
  float z = 1.0f;
  Vector3 pixel_nds = Vector3(x, y, z); // normalized device space
  Vector4 pixel_clip = Vector4(pixel_nds.x, pixel_nds.y, -1.0, 1.0); // homogeneous clip space
  Vector4 pixel_eye = projection.invert() * pixel_clip;
  pixel_eye = Vector4(pixel_eye.x, pixel_eye.y, -1.0, 0.0); // eye space
  Vector3 ray_world = (view.invert() * pixel_eye).xyz;
  return ray_world.normalize(); // direction in world space
}


Vector3 ray_plane_intersect(Vector3 rayOrigin, Vector3 rayDirection, Vector3 planePosition, Vector3 planeNormal)
{
  // https://stackoverflow.com/questions/23975555/how-to-do-ray-plane-intersection
  float denom = planeNormal.dot(rayDirection);
  if (fabs(denom) < EPSILON) {
    // No intersection, ray is paralell
    return Vector3(NAN, NAN, NAN);
  }
  float t = (planePosition - rayOrigin).dot(planeNormal) / denom;
  return rayOrigin + t * rayDirection;
}




#endif // MATH3D_H
