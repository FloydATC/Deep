#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "GFX.h"
#include "Texture.h"
#include "Vectors.h"

class Material
{
  public:
    Material();
    ~Material();

    void setName(std::string name);
    void setFilename(std::string filename);

    Vector4 getAmbientColor();
    Vector4 getDiffuseColor();
    Vector4 getSpecularColor();
    Vector4 getEmissiveColor();

    void setAmbientColor(float r, float g, float b);
    void setDiffuseColor(float r, float g, float b);
    void setSpecularColor(float r, float g, float b);
    void setEmissiveColor(float r, float g, float b);

    std::string name = "(no name)";
    std::string file = "(no file)";

  protected:

  private:
    void initialize();

    Vector4 color_a;
    Vector4 color_d;
    Vector4 color_s;
    Vector4 color_e;

    float specular_exponent;
    float optical_density;  // Light refraction 0.001 .. 10.0
    float opacity;          // 0..1, 1.0 = solid, 0.0 = transparent
    int illumination_mode;  // See below

    GLuint texture_ambient;
    GLuint texture_diffuse;
    GLuint texture_bump;

};

std::ostream& operator <<(std::ostream& stream, const Material* material);

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
