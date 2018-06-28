#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "vec.hpp"
#include "SOIL.h"

struct Group
{
public:
  Group(const std::string& name);

public:
  std::string                         m_name;

  std::vector<kmuvcl::math::vec3f>    m_vertices;
  std::vector<kmuvcl::math::vec3f>    m_normals;
  std::vector<kmuvcl::math::vec2f>    m_texcoords;
  std::string                         m_mtl_name;
};

struct Material
{
public:
  Material();
  Material(const std::string& name,
    kmuvcl::math::vec4f& ambient,
    kmuvcl::math::vec4f& diffuse,
    kmuvcl::math::vec4f& specular,
    float& shininess);

public:
  std::string           m_name;

  kmuvcl::math::vec4f   m_ambient;
  kmuvcl::math::vec4f   m_diffuse;
  kmuvcl::math::vec4f   m_specular;
  float                 m_shininess;
};

class Object
{
public:
  Object() {}

  int textureid;

  void draw(int loc_a_vertex, int loc_a_normal, int loc_a_texcoord,
    int loc_u_material_ambient, int loc_u_material_diffuse,
    int loc_u_material_specular, int loc_u_material_shininess);
  void draw(int loc_a_vertex, int loc_a_normal,
  int loc_u_material_ambient, int loc_u_material_diffuse,
  int loc_u_material_specular, int loc_u_material_shininess);

  void print();

  void set_value(const kmuvcl::math::vec3f position);
  void set_minmax();
  void set_center_position();

	bool load_simple_obj(const std::string& filename, std::string name);
  bool load_simple_mtl(const std::string& filename);
  void load_texture(const std::string& filename);

  kmuvcl::math::vec3f get_position();
  GLfloat get_max_length();
  kmuvcl::math::vec3f get_center_position();
  std::string get_objName();


private:
  std::string PATH;
  std::vector<Group>              m_groups;
  std::map<std::string, Material> m_materials;

  kmuvcl::math::vec3f position_;
  kmuvcl::math::vec3f minVertex;
  kmuvcl::math::vec3f maxVertex;
  kmuvcl::math::vec3f center_position;
  GLfloat   max_length, rotate_;
  std::string objectName;
};
