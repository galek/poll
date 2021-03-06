#include "poll.h"
#include "poll_plugin.h"
#include "plugin_debug.h"
#include "plugin_light_tool.h"
#include "plugin_node_tool.h"
#include "plugin_firstperson_cam.h"
#include "physics.h"
#include <iostream>


Poll poll;
Physics_Character_Controller *character;
unsigned int direction = 0;
Armature *armature = nullptr;

Node *panda;

#if 0
static void joystick_axis_motion_cb(SDL_JoyAxisEvent *ev)
{
  static int joystick_x = 0;
  static int joystick_y = 0;
  Window &window = poll.window_get();
  float r, angle;

  direction |= PHYSICS_DIRECTION_ROTATE;
  direction |= PHYSICS_DIRECTION_FORWARD;

  switch (ev->axis) {
    case 0: // x-axisa
      joystick_x = ev->value;
      break;
    case 1: // y-axis
      joystick_y = ev->value;
      joystick_y = joystick_y * (-1.f);
      break;
    default:
      break;
  }

  angle = window.joystick_angle_get(joystick_x, joystick_y, &r);
  if (isnan(angle))
    return;

  if (r > 0.1) {
    character->joystick_angle_set(angle);
    character->move(static_cast<Physics_Direction>(direction));
  } else {
    direction &= ~PHYSICS_DIRECTION_ROTATE;
    direction &= ~PHYSICS_DIRECTION_FORWARD;
    character->joystick_angle_set(0.f);
    character->move(static_cast<Physics_Direction>(direction));
  }

}


static void joystick_button_pressed_cb(SDL_JoyButtonEvent *ev)
{
  std::cout << "Button: " << ev->button << std::endl;

  switch (ev->button) {
    case 1: // B button
      character->jump();
      break;
    default:
      break;
  }

}


static void joystick_button_released_cb(SDL_JoyButtonEvent *ev)
{
}


static void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  /*

  if (console.active()) 
    return;

  switch (keysym->sym) {
    case SDLK_SPACE:
      character->jump();
      break;
    case SDLK_UP:
      direction |= PHYSICS_DIRECTION_FORWARD;
      break;
    case SDLK_DOWN:
      direction |= PHYSICS_DIRECTION_BACK;
      break;
    case SDLK_RIGHT:
      direction |= PHYSICS_DIRECTION_RIGHT;
      break;
    case SDLK_LEFT:
      direction |= PHYSICS_DIRECTION_LEFT;
      break;
    case SDLK_1:
      direction |= PHYSICS_DIRECTION_STRAFE_LEFT;
      break;
    case SDLK_2:
      direction |= PHYSICS_DIRECTION_STRAFE_RIGHT;
      break;
    default:
      break;
  }

  character->move(static_cast<Physics_Direction>(direction));
  */
}


static void keyboard_released_cb(SDL_Keysym *keysym)
{
  /*
  Console &console = p->console_get();

  if (console.active()) 
    return;


  switch (keysym->sym) {
    case SDLK_UP:
      direction &= ~PHYSICS_DIRECTION_FORWARD;
      break;
    case SDLK_DOWN:
      direction &= ~PHYSICS_DIRECTION_BACK;
      break;
    case SDLK_RIGHT:
      direction &= ~PHYSICS_DIRECTION_RIGHT;
      break;
    case SDLK_LEFT:
      direction &= ~PHYSICS_DIRECTION_LEFT;
      break;
    case SDLK_1:
      direction &= ~PHYSICS_DIRECTION_STRAFE_LEFT;
      break;
    case SDLK_2:
      direction &= ~PHYSICS_DIRECTION_STRAFE_RIGHT;
      break;
    default:
      break;
  }

  character->move(static_cast<Physics_Direction>(direction));

  */
}


static void physics_update()
{
  static Physics_Character_State last_state;
  Physics_Character_State cur_state = character->state_get();

  if (last_state == cur_state) return;

  switch (cur_state) {
    case CHARACTER_STATE_IDLE:
      armature->keyframe_range_activate("idle");
      break;
    case CHARACTER_STATE_MOVING:
      armature->keyframe_range_activate("run");
      break;
    case CHARACTER_STATE_JUMPING:
      armature->keyframe_range_activate("jump");
      break;
    default:
      break;
  }
  last_state = cur_state;
}
#endif



int main() 
{
  Scene &scene = poll.scene_get();
  Physics &physics = scene.physics_get();

  /*
  window.joystick_axis_motion_callback_set(joystick_axis_motion_cb);
  window.joystick_pressed_callback_set(joystick_button_pressed_cb);
  window.joystick_released_callback_set(joystick_button_released_cb);
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);
  window.keyboard_released_callback_set(keyboard_released_cb);
  physics.custom_step_callback_set(physics_update);
*/

  Node *camera_node = scene.node_camera_get();

  auto plugin_debug = std::unique_ptr<Plugin_Debug>(new Plugin_Debug(poll.console_get(), scene));
  auto plugin_light_tool = std::unique_ptr<Plugin_Light_Tool>(new Plugin_Light_Tool(poll.console_get(), scene));
  auto plugin_node_tool = std::unique_ptr<Plugin_Node_Tool>(new Plugin_Node_Tool(poll.console_get(), scene, 5.f));
  auto plugin_firstperson_camera = std::unique_ptr<Plugin_Firstperson_Camera>(new Plugin_Firstperson_Camera(poll.console_get(), scene, camera_node));
  poll.plugin_add(*plugin_debug);
  poll.plugin_add(*plugin_light_tool);
  poll.plugin_add(*plugin_firstperson_camera);
  poll.plugin_add(*plugin_node_tool);

  Node &root = scene.node_root_get();
  root.scale(scene, glm::vec3(0.1, 0.1, 0.1));

  Node &node = scene.load("data/", "orientation.dae", MODEL_IMPORT_DEFAULT | MODEL_IMPORT_BLENDER_FIX);
  node.translate(scene, vec3(0, 5, 0));


  Node &box = *scene.node_find(&node, "box");
  box.shadow_cast_set(scene, false);

  Node &suzanne_center = *scene.node_find(&node, "Suzanne_center");
 // suzanne_center.scale(scene, vec3(1, 1, 1));

  auto suzanne_center_shape = std::unique_ptr<Physics_Convex_Hull_Shape>(new Physics_Convex_Hull_Shape(suzanne_center));
  Physics_Rigidbody *suzanne_center_rigidbody = suzanne_center.physics_rigidbody_create(scene);
  suzanne_center_rigidbody->create(physics, *suzanne_center_shape, Physics_Rigidbody::KINEMATIC, 1);

  Node &suzanne_translated = *scene.node_find(&node, "Suzanne_translated");
  auto suzanne_translated_shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(suzanne_translated));
  Physics_Rigidbody *suzeanne_translated_rigidbody = suzanne_translated.physics_rigidbody_create(scene);
  suzeanne_translated_rigidbody->create(physics, *suzanne_translated_shape, Physics_Rigidbody::KINEMATIC, 1);
  suzanne_translated.scale(scene, vec3(3, 3, 3));

  Node &sphere = *scene.node_find(&node, "Sphere_ms");
  glm::vec3 v = vec3(1.f, 1.f, 1.f);
  auto sphere_shape = std::unique_ptr<Physics_Box_Shape>(new Physics_Box_Shape(v));
  Physics_Rigidbody *sphere_rigidbody = sphere.physics_rigidbody_create(scene);
  sphere_rigidbody->create(physics, *sphere_shape, Physics_Rigidbody::KINEMATIC, 1);


  Node &room = scene.load("data/game_assets/", "Room.dae", MODEL_IMPORT_OPTIMIZED);
  std::vector<std::unique_ptr<Physics_Triangle_Mesh_Shape>> shapes;
  for (auto &child: room.children_get()) {
    auto shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(*child));
    Physics_Rigidbody *rigidbody = child->physics_rigidbody_create(scene);
    if (rigidbody)
      rigidbody->create(scene.physics_get(), *shape, Physics_Rigidbody::DYNAMIC, 0);
    shapes.push_back(std::move(shape));
  }


  {
    //Node &node = scene.load("data/", "cone.dae", MODEL_IMPORT_OPTIMIZED);
  //  node.physics_rigidbody_create(scene, false, Physics_Rigidbody::CONVEX_HULL, Physics_Rigidbody::DYNAMIC, 1.f);
   // node.translate(scene, glm::vec3(0, 0, -20));
  }

  Node &sphere_node= scene.load("data/", "sphere.obj", MODEL_IMPORT_OPTIMIZED);
  sphere_node.translate(scene, glm::vec3(-3, 4, 3));
  sphere_node.scale(scene, glm::vec3(4, 4, 4));
  auto shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(sphere_node));
  Physics_Rigidbody *rigidbody = sphere_node.physics_rigidbody_create(scene);
  if (rigidbody)
    rigidbody->create(scene.physics_get(), *shape, Physics_Rigidbody::DYNAMIC, 1);

#if 0
  Node &node = scene.load("data/", "orientation.dae", MODEL_IMPORT_DEFAULT | MODEL_IMPORT_BLENDER_FIX);
  node.translate(scene, vec3(0, 5, 0));

  Node &suzanne_center = *scene.node_find(&node, "Suzanne_center");
  auto suzanne_center_shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(suzanne_center));
  Physics_Rigidbody *suzanne_center_rigidbody = suzanne_center.physics_rigidbody_create(scene);
  suzanne_center_rigidbody->create(physics, *suzanne_center_shape, Physics_Rigidbody::KINEMATIC, 1);
  suzanne_center.scale(scene, vec3(6, 2, 6));

  Node &suzanne_translated = *scene.node_find(&node, "Suzanne_translated");
  auto suzanne_translated_shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(suzanne_translated));
  Physics_Rigidbody *suzeanne_translated_rigidbody = suzanne_translated.physics_rigidbody_create(scene);
  suzeanne_translated_rigidbody->create(physics, *suzanne_translated_shape, Physics_Rigidbody::KINEMATIC, 1);
  suzanne_translated.scale(scene, vec3(3, 3, 3));

  Node &sphere = *scene.node_find(&node, "Sphere_ms");
  glm::vec3 v = vec3(1.f, 1.f, 1.f);
  auto sphere_shape = std::unique_ptr<Physics_Box_Shape>(new Physics_Box_Shape(v));
  Physics_Rigidbody *sphere_rigidbody = sphere.physics_rigidbody_create(scene);
  sphere_rigidbody->create(physics, *sphere_shape, Physics_Rigidbody::KINEMATIC, 1);

#endif

  /* Setup panda character */
  {
  //  Node &panda_root = scene.load("data/game_assets/characters/panda/", "PandaSingle.dae", MODEL_IMPORT_OPTIMIZED);
  //  panda_root.physics_rigidbody_create(scene, true, Physics_Rigidbody::CONVEX_HULL, Physics_Rigidbody::DYNAMIC, 0.f);

    /*
      Physics_Rigidbody *rigidbody = scene.node_find(&panda_root, "Panda")->physics_rigidbody_get();
      if (rigidbody) {
        rigidbody->mass_set(&physics, 1);
      }
*/
    /*
    Node &panda_collision_root = scene.load(glcontext, "data/game_assets/characters/panda/", 
        "Panda_convex_hull.dae", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);
    panda = scene.node_find(&panda_root, "Panda");
    Node *panda_collision = scene.node_find(&panda_collision_root, "Panda_convex_hull");
    if (panda && panda_collision) {
      armature = panda->armature_get();
      character = physics.character_controller_add(*panda, *panda_collision);
      armature->keyframe_range_set("bind", 0, 0);
      armature->keyframe_range_set("idle", 1, 3);
      armature->keyframe_range_set("walk", 4, 10);
      armature->keyframe_range_set("run", 11, 18);
      armature->keyframe_range_set("jump", 22, 22); // Only include 1 frame for now.
      armature->keyframe_range_set("collapse", 24, 33); 
    } else {
      std::cout << "Could not find node" << std::endl;
    }
    */
  }


  /* Setup lights */
  {

    glm::vec3 light_positions[4] = {
      glm::vec3(-5, 1.5, 4.5),
      glm::vec3(-5, 1.5, -4.5),
      glm::vec3(5, 1.5, 4.5),
      glm::vec3(5, 1.5, -4.5)
    };

    glm::vec3 light_color[4] = {
      glm::vec3(1, 0, 0),
      glm::vec3(0, 1, 0),
      glm::vec3(0, 0, 1),
      glm::vec3(1, 0.5, 0)
    };

    for (int i = 0; i < 4; i++) {
      Node *node = scene.node_create("Light_Point_" + std::to_string(i));
      Light *light = node->light_create(scene, Light::POINT);
      light->properties_color_set(light_color[i]);
      node->translate(scene, light_positions[i]);
      node->scale(scene, glm::vec3(30, 30, 30));
    }

    {
      Node *node = scene.node_create("Light_Directionl_Global");
      node->translate(scene, glm::vec3(0, 5, 0));
      Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
      light->properties_direction_set(glm::vec3(0, -1, -1));
      light->properties_color_set(glm::vec3(0.5, 0.5, 0.5));
    }

    {
      Node *node = scene.node_create("Light_Spot");
      Light *light = node->light_create(scene, Light::SPOT);
      node->translate(scene, glm::vec3(-4, 1.5, 0));
      node->scale(scene, glm::vec3(30, 30, 30));
      light->properties_color_set(glm::vec3(1, 0, 0));
    }


  }

  //scene.physics_get().pause();

  poll.run();
  poll.term();

  return true;
}
