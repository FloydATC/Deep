#ifndef MESH3D_H
#define MESH3D_H

#include <string>
#include <vector>

#include "GFX.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Texture.h"
#include "Vectors.h"
#include "3D/Face3D.h"
#include "3D/Light3D.h"
#include "3D/Vertex3D.h"

class ShadowVolume3D; // Forward declaration

class Mesh3D
{
  public:
    // No public ctor
    virtual ~Mesh3D();

    virtual void renderAmbient(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader) = 0;
    virtual void renderLight(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader, Light3D* light) = 0;

    void setName(std::string name);
    std::string getName();
    void setFilename(std::string filename);
    std::string getFilename();

    virtual void setTexture(GLuint texture);

    virtual void setDecalTexture(GLuint texture);
    void setDecalTexture(Texture* texture) { setDecalTexture(texture->id()); }
    virtual void setDecalPosition(Vector2 position);

    virtual void setMaterial(Material* material);
    Material* getMaterial();

    virtual void setShader(ShaderProgram* shader);
    ShaderProgram* getShader();

    void setBounds(Mesh3D* box);
    Mesh3D* getBounds();

    uint32_t addVertex(Vertex3D vertex);
    uint32_t addFace(Face3D face);
    void findAdjacentFaces();

    bool bounds_enabled;

    bool isEnabled();
    void enableShadow();
    void disableShadow();
    virtual bool castsShadow();
    virtual void generateShadowVolume(Light3D* light, Matrix4 model) {}
    virtual void renderShadowVolume(Matrix4 proj, Matrix4 view, Matrix4 model, ShaderProgram* shader) {}
    virtual void destroyShadowVolume() {}

    void show();
    void hide();

    bool debug;

    std::string file;
    std::string name;
    int serial;

    static int mesh_serial_no;

    std::vector<Vertex3D> vertices; // for shadow volumes
    std::vector<Face3D> faces; // for shadow volumes
    



  protected:
    Mesh3D(); // No public constructor

    Material*       material;
    Mesh3D*         bounds;
    ShaderProgram*  shader;

    virtual void initialize(ShaderProgram* shader);

    friend class Obj3DLoader;
    virtual void set_v(float* v, int num_vertices);
    void set_vt(float* v, int num_vertices);
    void set_vn(float* v, int num_vertices);


    int count_v;
    int count_vt;
    int count_vn;

    GLuint vao;
    GLuint vbo_v;
    GLuint vbo_vt;
    GLuint vbo_vn;
    GLuint texture;

    bool texture_set;
    bool render_enabled;
    bool cast_shadow;

    Material* my_material(Material* standard);
    ShaderProgram* my_shader(ShaderProgram* standard);

    void bind_vao();
    void unbind_vao();
    void bind_vbo(GLuint vbo);

    bool initialized;

    GLuint decal_texture;
    bool decal_texture_set;
    Vector2 decal_position;
    bool decal_position_set;

    ShadowVolume3D* shadow;

  private:
};



std::ostream& operator <<(std::ostream& stream, const Mesh3D* mesh);

#endif // MESH3D_H
