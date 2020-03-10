#ifndef MESH3D_H
#define MESH3D_H

#include <string>

#include "GFX.h"
#include "ShaderProgram.h"
#include "Material.h"


class Mesh3D
{
  public:
    // No public ctor
    virtual ~Mesh3D();

    virtual void render(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader) = 0;

    void setName(std::string name);
    std::string getName();
    void setFilename(std::string filename);
    std::string getFilename();

    virtual void setTexture(GLuint texture);

    virtual void setMaterial(Material* material);
    Material* getMaterial();

    virtual void setShader(ShaderProgram* shader);
    ShaderProgram* getShader();

    void setBounds(Mesh3D* box);
    Mesh3D* getBounds();

    bool bounds_enabled;

    bool isEnabled();
    void show();
    void hide();

    bool debug;

    std::string file;
    std::string name;
    int serial;

    static int mesh_serial_no;


  protected:
    Mesh3D();

    Material*       material;
    Mesh3D*         bounds;
    ShaderProgram*  shader;

    void initialize(ShaderProgram* shader);

    friend class Obj3DLoader;
    void set_v(float* v, int num_vertices);
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

    Material* my_material(Material* standard);
    ShaderProgram* my_shader(ShaderProgram* standard);

    void bind_vao();
    void unbind_vao();
    void bind_vbo(GLuint vbo);

    bool initialized;



  private:
};



std::ostream& operator <<(std::ostream& stream, const Mesh3D* mesh);

#endif // MESH3D_H
