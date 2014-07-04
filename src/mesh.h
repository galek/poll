#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gldefaults.h"
#include "vertex.h"
#include <vector>

class Mesh {
  private:

  public:
    std::vector<Vertex> vertices;
    std::vector<GLshort> indices;
    glm::mat4 model;
    glm::mat4 scale_matrix;

    Mesh();
    ~Mesh();

    void                    buffer_data_get(std::vector<glm::vec4> *vertices_ptr, std::vector<glm::vec4> *normals_ptr,
                                           std::vector<glm::vec4> *tangent_ptr, std::vector<glm::vec4> *bitangent_ptr,
                                           std::vector<glm::vec4> *weights_ptr, std::vector<glm::ivec4> *bone_indices_ptr,
                                           std::vector<glm::vec2> *uv_ptr, std::vector<GLshort> *indices_ptr);
    void                    print(const int indent_level);
    unsigned int            num_indices_get();
    unsigned int            num_vertices_get();
    std::vector<glm::vec4>  vertices_get(bool scale);
};

