#include "mesh.h"
#include "utils.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Mesh::Mesh():
  model(glm::mat4(1.0))
{
}

Mesh::~Mesh(void)
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Mesh::buffer_data_get(std::vector<glm::vec4> *vertices_ptr, 
    std::vector<glm::vec4> *normals_ptr,
    std::vector<glm::vec4> *tangent_ptr,
    std::vector<glm::vec4> *bitangent_ptr,
    std::vector<glm::vec4> *weights_ptr,
    std::vector<glm::ivec4> *bone_indices_ptr,
    std::vector<glm::vec2> *uv_ptr,
    std::vector<GLshort> *indices_ptr)
{
  std::vector<glm::vec4> tempVert;
  std::vector<glm::vec4> tempNormal;
  std::vector<glm::vec4> temp_tangent;
  std::vector<glm::vec4> temp_bitangent;
  std::vector<glm::vec4> tempWeight;
  std::vector<glm::ivec4> tempBoneIndex;
  std::vector<glm::vec2> tempUv;
  size_t n = vertices.size();

  for (size_t i = 0; i < n; i++) {
    //std::cout << vertices[i].position.x << ", " << vertices[i].position.y << ", " << vertices[i].position.z << std::endl;
    Vertex v;
    v.position.x = vertices[i].position.x;
    v.position.y = vertices[i].position.y;
    v.position.z = vertices[i].position.z;

    tempVert.push_back(glm::vec4(v.position, 1.0));
    tempNormal.push_back(glm::vec4(vertices[i].normal, 1.0));
    temp_tangent.push_back(glm::vec4(vertices[i].tangent, 1.0));
    temp_bitangent.push_back(glm::vec4(vertices[i].bitangent, 1.0));
    tempWeight.push_back(vertices[i].weights);
    tempBoneIndex.push_back(vertices[i].bones);
    tempUv.push_back(vertices[i].uv);
  }

  *vertices_ptr = tempVert;
  *normals_ptr = tempNormal;
  *tangent_ptr = temp_tangent;
  *bitangent_ptr = temp_bitangent;
  *weights_ptr = tempWeight;
  *bone_indices_ptr = tempBoneIndex;
  *uv_ptr = tempUv;
  *indices_ptr = indices;
}


const std::vector<GLshort> &Mesh::indices_get() const
{
  return indices;
}


void Mesh::print(const int indent_level)
{
  indent(std::cout, indent_level);

  std::cout << "\tvertices: " << num_vertices_get() << ", indices: " << num_indices_get() << std::endl;
}


unsigned int Mesh::num_indices_get() {
  return indices.size();
}


unsigned int Mesh::num_vertices_get() {
  return vertices.size();
}


const std::vector<glm::vec3> &Mesh::positions_get() const
{
  return positions;
}


std::vector<glm::vec3> Mesh::vertices_get(bool scale)
{
  std::vector<glm::vec3> temp_vertices;
  size_t n = vertices.size();

  for (size_t i = 0; i < n; i++) {
    Vertex v;
    glm::vec3 position;
    v.position.x = vertices[i].position.x;
    v.position.y = vertices[i].position.y;
    v.position.z = vertices[i].position.z;
    std::cout << glm::to_string(v.position) << std::endl;

    if (scale) {
      //position = m::vec4(v.position, 1.0) * scale_matrix;
    } else {
      //position = glm::vec3(glm::vec4(v.position, 1.0) * model);
      position = v.position;
    }

    temp_vertices.push_back(position);
  }

  return temp_vertices;
}


