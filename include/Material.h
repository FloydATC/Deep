#ifndef MATERIAL_H
#define MATERIAL_H

#include "GFX.h"
#include "Texture.h"
#include "Vectors.h"

class Material
{
  public:
    Material();
    ~Material();

    Vector3 color_ambient;
    Vector3 color_diffuse;
    Vector3 color_specular;
    Vector3 color_emissive;
    float specular_exponent;
    float optical_density; // Light refraction 0.001 .. 10.0
    float opacity; // 0..1, 1.0 = solid, 0.0 = transparent
    int illumination_mode; // See below

    GLuint texture_ambient;
    GLuint texture_diffuse;
    GLuint texture_bump;

  protected:

  private:
    void initialize();
};

#endif // MATERIAL_H

// http://paulbourke.net/dataformats/mtl/

/*

0. Color on and Ambient off
1. Color on and Ambient on
2. Highlight on
3. Reflection on and Ray trace on
4. Transparency: Glass on, Reflection: Ray trace on
5. Reflection: Fresnel on and Ray trace on
6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
8. Reflection on and Ray trace off
9. Transparency: Glass on, Reflection: Ray trace off
10. Casts shadows onto invisible surfaces

*/

/*

.mtl file sample contents

Ns 225.000000
Ka 1.000000 1.000000 1.000000
Kd 0.800000 0.800000 0.800000
Ks 0.500000 0.500000 0.500000
Ke 0.000000 0.000000 0.000000
Ni 1.450000
d 1.000000
illum 2
map_Kd C:\\Users\\floyd\\Documents\\Cpp\\Deep\\textures\\test256.png

*/
