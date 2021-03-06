#include "scene.h"
#include "node.h"
#include "utils.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Node::Node(const std::string &node_name)
{
  name_set(node_name);
} 


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

Aabb &Node::aabb_get()
{
  if (!rigidbody) {
    POLL_ERROR(std::cerr, "no rigidbody attached to node, cannot get aabb");
    return *aabb;
  }

  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
    aabb = rigidbody->aabb_get(true);
  }
  aabb = rigidbody->aabb_get(false);

  return *aabb;
}


void  Node::active_set(Scene &scene, const bool flag)
{
  this->active = flag;

  Physics_Rigidbody *rigidbody = physics_rigidbody_get();
  if (rigidbody) {
    if (flag) {
      scene.physics_get().rigidbody_add(rigidbody);
    } else {
      scene.physics_get().rigidbody_delete(rigidbody);
    }
  }

  for (auto &child : children_get()) {
    child->active_set(scene, flag);
  }
  
}


bool Node::active_get()
{
  return active;
}


Armature *Node::armature_get()
{
  return armature;
}

void Node::armature_set(Armature *armature)
{
  this->armature = armature;

}


void Node::callback_draw_call()
{
  if (!callback_draw)
    return;

  callback_draw(*this);
}


void Node::callback_draw_set(const std::function <void (Node &node)> callback)
{
  this->callback_draw = callback;
}


void Node::callback_raycast_collide_call(vec3 &position)
{
  if (!callback_raycast_collide)
    return;

  callback_raycast_collide(*this, position);

}


void Node::callback_raycast_collide_set(const std::function <void (Node &node, vec3 &position)> callback)
{
  this->callback_raycast_collide = callback;
}



Camera *Node::camera_create(Assets &assets)
{
  std::unique_ptr<Camera> camera(new Camera());
  Camera *camera_ptr = camera.get();
  camera_set(camera_ptr);
  assets.camera_add(std::move(camera));
  return camera_ptr;
}


Camera *Node::camera_get()
{
  return camera;
}


void Node::camera_set(Camera *camera)
{
  this->camera = camera;
}


Node_List const &Node::children_get() const
{
  return children;
}


void Node::child_add(std::unique_ptr<Node> &&node, int level) 
{
  node->parent = this;
  node->tree_level = level;
  children.push_back(std::move(node));
}




Light *Node::light_create(Scene &scene, const unsigned int lamp_type, const unsigned int illumination_type)
{
  Assets &assets = scene.assets_get();
  Stock_Nodes &stock_nodes = assets.stock_nodes_get();
  Node *node_ptr = nullptr;

  if (illumination_type == Light::GLOBAL) {
    node_ptr = stock_nodes.screen_quad_get();
  } else {
    node_ptr = stock_nodes.sphere_get();
  }

  if (!node_ptr) {
    std::cout << "Error: Light type was not set" << std::endl;
    return nullptr;
  }

  std::unique_ptr<Light> light(new Light());
  Light *light_ptr = light.get();
  light_ptr->properties_type_set(lamp_type);
  light_ptr->illumination_type_set(illumination_type);
  mesh_set(node_ptr->mesh_get());
  light_set(light_ptr);

  auto plugins = scene.plugins_get();
  for (auto &plugin : plugins) {
    plugin->cb_light_create(this);
  }

  assets.light_active_add(std::move(light));

  return light_ptr;
}


Light *Node::light_get()
{
  return light;
}


void Node::light_set(Light *light)
{
  if (light)
    light->node_ptr_set(this);

  this->light = light;
}


void Node::link_set(Node *node)
{
  POLL_DEBUG(std::cout, "Setting node: " << name_get() << " to link against: " << node->name_get());
  this->link = node;
}


Node *Node::link_get()
{
  return link;
}


Node *Node::parent_get()
{
  return parent;
}


vec3 Node::position_get()
{
  mat4 m = transform_global_translate_get();
  return vec3(m[3][0], m[3][1], m[3][2]);
}


vec3 Node::position_local_get()
{
  mat4 m = transform_translate_get();
  return vec3(m[3][0], m[3][1], m[3][2]);
}


Physics_Rigidbody *Node::physics_rigidbody_create(Scene &scene, bool recursive)
{
  Assets &assets = scene.assets_get();
  Physics_Rigidbody *rigidbody_ptr = nullptr;

  if (!mesh_get()) {
    POLL_ERROR(std::cerr, "Cannot create rigidbody, no mesh for node: " << name_get());
  } else {
    std::unique_ptr<Physics_Rigidbody> rigidbody(new Physics_Rigidbody());
    rigidbody_ptr = rigidbody.get();
    rigidbody_ptr->node_ptr_set(this);
    physics_rigidbody_set(rigidbody_ptr);
    assets.physics_rigidbody_add(std::move(rigidbody));
  }

  return rigidbody_ptr;
}


Physics_Rigidbody *Node::physics_rigidbody_get()
{
  return rigidbody;
}


void Node::physics_rigidbody_set(Physics_Rigidbody *rigidbody)
{
  this->rigidbody = rigidbody;
}


void Node::physics_rigidbody_update()
{
  Physics_Rigidbody *rigidbody = physics_rigidbody_get();
  if (rigidbody) {
    unsigned int type = rigidbody->type_get();

    if (type == Physics_Rigidbody::DYNAMIC) {
      POLL_ERROR(std::cerr, "Cannot manipulate a dynamic rigidbody after creation");
      return;
    }

    rigidbody->motionstate_update(this);
  }
}


const std::string &Node::name_get()
{
  return name;
}


void Node::name_set(const std::string &name)
{
  this->name = name;
}


Material *Node::material_create(Assets &assets)
{
  std::unique_ptr<Material> material(new Material());
  Material *material_ptr = material.get();
  material_set(material_ptr);
  assets.material_add(std::move(material));
  return material_ptr;
}


Material *Node::material_get()
{
  return material;
}


void Node::material_set(Material *material)
{
  this->material = material;
}


Mesh *Node::mesh_create(Scene &scene)
{
  Assets &assets = scene.assets_get();

  std::unique_ptr<Mesh> mesh(new Mesh());
  Mesh *mesh_ptr = mesh.get();
  mesh_set(mesh_ptr);

  assets.mesh_add(std::move(mesh));
  return mesh_ptr;
}


Mesh *Node::mesh_get()
{
  return mesh;
}


void Node::mesh_set(Mesh *mesh)
{
  this->mesh = mesh;
}

short Node::raycast_priority_get()
{
  return raycast_priority;
}


void Node::raycast_priority_set(const short priority)
{
  this->raycast_priority = priority;
}


void Node::rotate(Scene &scene, const float angle, const vec3 &v)
{
  vec3 r = v;

  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
    r = vec3(blender_transform_get() * vec4(v, 1));
  }
  transform_rotate = glm::rotate(transform_rotate, angle, r);
  scene.transform_update_global_recursive(this);
}



void Node::scale(Scene &scene, const vec3 &v)
{
  vec3 t = v;

  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
    t = vec3(v.x, v.z, v.y);
  }
  transform_scale = glm::scale(transform_scale, t);
  scene.transform_update_global_recursive(this);
}


void Node::scale_identity(Scene &scene, const vec3 &v)
{
  vec3 t = v;

  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
    t = vec3(v.x, v.z, v.y);
  }
  transform_scale = glm::scale(mat4(1.f), t);
  scene.transform_update_global_recursive(this);
}


glm::vec3 Node::scale_get()
{
  glm::vec3 diagonal(transform_scale[0][0], transform_scale[1][1], transform_scale[2][2]);
  return diagonal;
}


glm::vec3 Node::scale_global_get()
{
  return vec3(global_transform_scale[0][0], global_transform_scale[1][1], global_transform_scale[2][2]);
}


bool Node::shadow_cast_get()
{
  return shadow_cast;
}


void Node::shadow_cast_set(Scene &scene, const bool shadow_cast)
{
  this->shadow_cast = shadow_cast;
  scene.draw_nodes_remove(*this);
}


Text *Node::text_create(Font *font, Scene &scene)
{
  Assets &assets = scene.assets_get();
  std::unique_ptr<Text> text(new Text());
  Text *text_ptr = text.get();
  text_ptr->font_set(font);
  text_set(text_ptr);

  if (!mesh_get())
    mesh_create(scene);

  assets.text_add(std::move(text));
  return text_ptr;
}


Text *Node::text_get()
{
  return text;
}


void Node::text_set(Text *text)
{
  this->text = text;
}


void Node::translate(Scene &scene, const vec3 &v) 
{
  vec3 t = v;

  transform_translate = glm::translate(transform_translate, t);
  scene.transform_update_global_recursive(this);
}


void Node::translate_identity(Scene &scene, const vec3 &v) 
{
  vec3 t = v;

  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
  //  t = vec3(blender_transform_get() * vec4(v, 1));
  }

  transform_translate = glm::translate(mat4(1.f), t);
  scene.transform_update_global_recursive(this);
}


void Node::transform_inheritance_set(Transform_Inherit transform_inheritance)
{
  this->transform_inheritance = transform_inheritance;
}


Transform_Inherit Node::transform_inheritance_get()
{
  return transform_inheritance;
}


mat4 &Node::transform_global_get()
{
  return transform_global;
}


glm::mat4 Node::transform_full_update()
{
  glm::mat4 m = transform_global_translate_get() * transform_global_rotate_get() * transform_global_scale_get();
  glm::mat4 local = transform_translate_get() * transform_rotate_get() * transform_scale_get();

 // glm::mat4 m = transform_global_scale_get() * transform_global_rotate_get() * transform_global_translate_get();
 // glm::mat4 local = transform_scale_get() * transform_rotate_get() * transform_translate_get();
  transform_local_current_set(local);
  transform_global_set(m);
  physics_rigidbody_update();
  return m;
}


void Node::transform_global_set(const mat4 &transform)
{
  this->transform_global = transform;
}


void Node::transform_global_from_node_set(Node &node, const mat4 &transform)
{
  mat4 m = transform;

  this->transform_global = node.transform_global_translate_get() * 
    node.transform_global_rotate_get() * node.transform_global_scale_get() * m;
}

void Node::transform_local_current_set_only(const mat4 &transform)
{
  this->transform_local_current = transform;
}


void Node::transform_local_current_set(const mat4 &transform) 
{
  this->transform_local_current = transform;
}


mat4 &Node::transform_local_current_get()
{
  return transform_local_current;
}


void Node::transform_local_original_set(const mat4 &transform) 
{
  this->transform_local_original = transform;
}


mat4 &Node::transform_local_original_get()
{
  return transform_local_original;
}


mat4 Node::transform_rotate_get()
{
  return transform_rotate;
}


void Node::transform_rotate_set(glm::quat &q)
{
  mat4 rotation = glm::mat4_cast(q);
  
  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
    rotation = blender_transform_get() * rotation;
  }
  transform_rotate = rotation;
}


mat4 Node::transform_scale_get()
{
  return transform_scale;
}


void Node::transform_scale_set(glm::vec3 &v)
{
  vec3 t = v;

  /*
  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
    t = vec3(blender_transform_get() * vec4(v, 1));
  }
  */

  transform_scale = glm::scale(mat4(1.f), t);
}


mat4 Node::transform_global_scale_get()
{
  return global_transform_scale;
}


void Node::transform_global_rotate_set(const mat4 &transform)
{
  this->global_transform_rotate = transform;
}


mat4 Node::transform_global_rotate_get()
{
  return global_transform_rotate;
}


void Node::transform_global_scale_set(const mat4 &transform)
{
  this->global_transform_scale = transform;
}


mat4 Node::transform_global_translate_get()
{
  return global_transform_translate;
}


void Node::transform_global_translate_set(const mat4 &transform)
{
  this->global_transform_translate = transform;
}


void Node::transform_translate_set(glm::vec3 &v)
{
  vec3 t = v;

  if (import_options & MODEL_IMPORT_BLENDER_FIX) {
    t = vec3(blender_transform_get() * vec4(v, 1));
  }
  transform_translate = glm::translate(mat4(1.f), t);
}


mat4 Node::transform_translate_get()
{
  return transform_translate;
}


const int &Node::tree_level_get()
{
  return tree_level;
}


void Node::tree_level_set(const unsigned int &tree_level)
{
  this->tree_level = tree_level;
}
