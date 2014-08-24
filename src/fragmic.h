#pragma once

#include <SDL2/SDL_stdinc.h>            // for Uint32
#include <string>
#include "config.h"
#include "console.h"
#include "camera.h"                     
#include "glcontext.h"                  
#include "glshader.h"                   
#include "physics.h"
#include "scene.h"                      
#include "ui.h"
#include "window.h"                     


#define GLOBAL_CONFIG "./fragmic.conf"


class Fragmic {
  private:

    Config config;
    Console console;
    Physics physics;
    GLshader glshader_geometry;
    GLshader glshader_light;
    GLshader glshader_stencil;
    GLshader glshader_text;
    Scene scene;
    Ui ui;
    Window window;

    std::string fps_text;

    double       delta_time_get();
    std::string &fps_text_get();
    void         profile_fps(const double dt);

  public:
    Fragmic(const std::string &config_file = ""); 
    ~Fragmic(); 

    Assets      &assets_get();
    Config      &config_get();
    Console     &console_get();
    GLcontext   &glcontext_get();
    Physics     &physics_get();
    void         run();
    Scene       &scene_get();
    void         term();
    Window      &window_get();
};


