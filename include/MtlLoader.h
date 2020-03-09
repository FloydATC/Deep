#ifndef MTLLOADER_H
#define MTLLOADER_H

#include <string>

#include "Material.h"
#include "FileScanner.h"


class MtlLoader
{
  public:
    MtlLoader();
    ~MtlLoader();

    Material* load(std::string filename, std::string materialname);

  protected:

  private:
    int errors;
    bool current;
    FileScanner* scanner;
    Material* material;
    std::string filename;
    std::string materialname;

    void get_keyword();
    void get_newmtl();
    void get_Ns();
    void get_Ka();
    void get_Kd();
    void get_Ks();
    void get_Ke();
    void get_Ni();
    void get_d();
    void get_illum();
    void get_map_Kd();

};

#endif // MTLLOADER_H
