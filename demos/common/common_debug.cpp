#include "common.h"
#include "fragmic.h"


static void common_debug_keyboard_pressed_cb(SDL_Keysym *keysym);


static void common_debug_keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = f->physics_get();

  switch (keysym->sym) {
    case SDLK_f:
      physics.pause();
      break;
    case SDLK_o:
      physics.debug();
      break;
    default:
      break;
  }
}

void common_debug_use()
{
  Window &window = f->window_get();
  window.keyboard_pressed_callback_set(common_debug_keyboard_pressed_cb);
}