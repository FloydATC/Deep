#ifndef OBJ3DLOADER_H
#define OBJ3DLOADER_H

#include <string>

#include "IO/IOFile.h"
#include "Matrices.h"
#include "3D/Obj3D.h"
#include "3D/SubObject3D.h"
#include "3D/Vertex3D.h"
#include "FileScanner.h"
#include "MtlLoader.h"
#include "Material.h"
#include "IO/IOFile.h"



class Obj3DLoader
{
  public:
    Obj3DLoader();
    ~Obj3DLoader();

    Obj3D* load(std::string filename);

  protected:

  private:
    Obj3D* object;
    FileScanner* scanner;
    std::string filename;
    std::string mtllib_fname;

    std::vector<Vector3> indexed_v;
    std::vector<Vector2> indexed_vt;
    std::vector<Vector3> indexed_vn;

    std::vector<Vertex3D> linear_points;
    std::vector<int> unique_v;
    std::vector<int> subobject_start;
    std::vector<int> subobject_length;
    int subobject_no;

    std::vector<SubObject3D*> subobject_mesh;
    std::vector<Material*> subobject_material;
    Box3D* box;

    void reset();
    float* make_v_array();
    float* make_vt_array();
    float* make_vn_array();

    void begin_mesh(std::string filename, std::string name);
    SubObject3D* current_mesh;

    std::vector<Vertex3D> compute_vn(std::vector<Vertex3D> points);
    void add_triangle(std::vector<Vertex3D> face);
    void add_quad(std::vector<Vertex3D> face);

    std::vector<std::string> split_tokens(std::string str);
    std::vector<float> tokens_to_floats(std::vector<std::string>);
    void get_keyword();
    void get_mtllib();
    void get_usemtl();
    void get_f();
    void get_l();
    void get_s();
    void get_o();
    void get_v();
    void get_vt();
    void get_vn();
    void get_vp();

};

#endif // OBJ3DLOADER_H
