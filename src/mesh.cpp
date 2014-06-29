#include "mesh.h"
#include <iostream>

Mesh::Mesh():
  model(glm::mat4(1.0))
{
}

Mesh::~Mesh(void)
{
}


unsigned int Mesh::num_indices_get() {
  return indices.size();
}


void Mesh::buffer_data_get(std::vector<glm::vec4> *vertices_ptr, 
    std::vector<glm::vec4> *normals_ptr,
    std::vector<glm::vec4> *weights_ptr,
    std::vector<glm::ivec4> *bone_indices_ptr,
    std::vector<glm::vec2> *uv_ptr,
    std::vector<GLshort> *indices_ptr)
{
  std::vector<glm::vec4> tempVert;
  std::vector<glm::vec4> tempNormal;
  std::vector<glm::vec4> tempWeight;
  std::vector<glm::ivec4> tempBoneIndex;
  std::vector<glm::vec2> tempUv;
  std::vector<GLshort> tempIndices;
  size_t n = vertices.size();

  for (size_t i = 0; i < n; i++) {
    //std::cout << vertices[i].position.x << ", " << vertices[i].position.y << ", " << vertices[i].position.z << std::endl;
    tempVert.push_back(glm::vec4(vertices[i].position, 1.0));
    tempNormal.push_back(glm::vec4(vertices[i].normal, 1.0));
    tempWeight.push_back(vertices[i].weights);
    tempBoneIndex.push_back(vertices[i].bones);
    tempUv.push_back(vertices[i].uv);
  }

  for (size_t i = 0; i < indices.size(); i++) {
    tempIndices.push_back(indices[i]);
  }

  *vertices_ptr = tempVert;
  *normals_ptr = tempNormal;
  *weights_ptr = tempWeight;
  *bone_indices_ptr = tempBoneIndex;
  *uv_ptr = tempUv;
  *indices_ptr = tempIndices;
}
