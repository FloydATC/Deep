#include "MtlLoader.h"

#include <iostream>

#include "Vectors.h"

#define DEBUG_TRACE_LOADER

MtlLoader::MtlLoader()
{
  //ctor
  initialize();
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader" << this << " created" << std::endl;
#endif
}

MtlLoader::~MtlLoader()
{
  //dtor
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader" << this << " destroyed" << std::endl;
#endif
}


void MtlLoader::initialize()
{
  this->material = nullptr;
  this->materialname = "";
  this->filename = "";
  this->errors = 0;
  this->current = false;
}

Material* MtlLoader::load(std::string filename, std::string materialname)
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader" << this << "::load() filename=" << filename << " materialname=" << materialname << std::endl;
#endif
  initialize();
  this->filename = filename;
  this->materialname = materialname;
  this->scanner = new FileScanner();
  if (scanner->load(filename) == false) {
    delete this->scanner;
    std::cerr << "MtlLoader" << this << "::load() error loading " << filename << std::endl;
    return nullptr;
  }
  this->material = new Material();

  while (!scanner->is_eof()) {
    scanner->skip_whitespace(true);
    if (scanner->is_alpha()) { get_keyword(); continue; }
    std::cerr << "MtlLoader" << this << "::load() unexpected character code " << (int)this->scanner->what() << " at " << this->scanner->where() << std::endl;
    this->errors++;
    scanner->advance();
  }
  if (errors > 0) {
    std::cerr << "MtlLoader::load() errors parsing " << filename << ": " << errors << std::endl;
  }

  delete this->scanner;
  return this->material;
}



void MtlLoader::get_keyword()
{
  std::string keyword = scanner->get_keyword();
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_keyword() got keyword=" << keyword << std::endl;
#endif
  if (keyword == "newmtl") { get_newmtl(); return; } // New material
  if (keyword == "Ns") { get_Ns(); return; } // ? *IGNORED*
  if (keyword == "Ka") { get_Ka(); return; } // Ambient color
  if (keyword == "Kd") { get_Kd(); return; } // Diffuse color
  if (keyword == "Ks") { get_Ks(); return; } // Specular color
  if (keyword == "Ke") { get_Ke(); return; } // Emissive color
  if (keyword == "Ni") { get_Ni(); return; } // ? *IGNORED*
  if (keyword == "d") { get_d(); return; } // ? *IGNORED*
  if (keyword == "illum") { get_illum(); return; } // ? *IGNORED*
  if (keyword == "map_Kd") { get_map_Kd(); return; } // Diffuse texture map

  std::cerr << "MtlLoader::get_keyword() unhandled keyword=" << keyword << std::endl;
}


void MtlLoader::get_newmtl()
{
  std::string found = scanner->get_string();
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_newmtl() found='" << found << "' want='" << this->materialname << "' " << scanner->where() << std::endl;
#endif
  this->current = (this->materialname == found);

  // Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume_eol();

}


void MtlLoader::get_Ns()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_Ns() " << scanner->where() << std::endl;
#endif
  // Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume_eol();

}

void MtlLoader::get_Ka()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_Ka() " << scanner->where() << std::endl;
#endif
  if (this->current) {
    this->material->color_ambient = Vector3(
      scanner->get_float(),
      scanner->get_float(),
      scanner->get_float()
    );
  } else {
    ignore();
  }
}

void MtlLoader::get_Kd()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_Kd() " << scanner->where() << std::endl;
#endif
  if (this->current) {
    this->material->color_diffuse = Vector3(
      scanner->get_float(),
      scanner->get_float(),
      scanner->get_float()
    );
  } else {
    ignore();
  }
}

void MtlLoader::get_Ks()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_Ks() " << scanner->where() << std::endl;
#endif
  if (this->current) {
    this->material->color_specular = Vector3(
      scanner->get_float(),
      scanner->get_float(),
      scanner->get_float()
    );
  } else {
    ignore();
  }
}

void MtlLoader::get_Ke()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_Ke() " << scanner->where() << std::endl;
#endif
  if (this->current) {
    this->material->color_emissive = Vector3(
      scanner->get_float(),
      scanner->get_float(),
      scanner->get_float()
    );
  } else {
    ignore();
  }
}

void MtlLoader::get_Ni()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_Ni() " << scanner->where() << std::endl;
#endif

  ignore();
}

void MtlLoader::get_d()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_d() " << scanner->where() << std::endl;
#endif

  ignore();
}

void MtlLoader::get_illum()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_illum() " << scanner->where() << std::endl;
#endif

  ignore();
}

void MtlLoader::get_map_Kd()
{
#ifdef DEBUG_TRACE_LOADER
  std::cout << "MtlLoader::get_map_Kd() " << scanner->where() << std::endl;
#endif

  ignore();
}


void MtlLoader::ignore()
{
  // Skip over the remainder of current line
  while (!scanner->is_eof() && !scanner->is_eol()) scanner->advance();
  scanner->consume_eol();
}
