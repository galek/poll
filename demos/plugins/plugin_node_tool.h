#pragma once

#include "poll.h"
#include "poll_plugin.h"
#include "raycast.h"

#include <memory>

using glm::vec3;


class Plugin_Node_Tool: public Poll_Plugin
{
  private:
    Console *console = nullptr;
    Scene *scene = nullptr;

    Raycast raycast;

    bool mouse_down = false;
    std::shared_ptr<Raycast_Hitpoint> hitpoint_last;


  public:
    Plugin_Node_Tool(Console &console, Scene &scene);
    void cb_mouse_pressed(SDL_MouseButtonEvent *ev);
    void cb_mouse_released(SDL_MouseButtonEvent *ev);
    void cb_mouse_motion(SDL_MouseMotionEvent *ev);

};