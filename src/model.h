#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "armature.h"
#include "assets.h"
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "transform.h"
#include "utils.h"


class Model {
  private:
    typedef std::map <const aiBone *, Bone *> BoneForAssimpBone;
    typedef std::map <std::string, Node *> NodeMap;
    
    std::vector<Material *>  materials;
    NodeMap                  nodes;
    const aiScene           *scene;
    std::string              prefix;
    Armature                *armaturePtr;

    void   ai_mat_copy(const aiMatrix4x4 *from, glm::mat4 &to);
    void   bone_map_create(Assets &assets, BoneForAssimpBone &boneForAssimpBone);
    void   materials_parse(Assets &assets);
    void   mesh_create_all(Assets &assets, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone);
    void   mesh_create(Assets &assets, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone);
    Node  *node_map_create(const aiNode &node, Node *parent, int level);
    void   key_frames_parse(void);

  public:
    Model();
    ~Model();

    Node *load(Assets &assets, Node &root, const std::string &prefix, const std::string &filename);
};
