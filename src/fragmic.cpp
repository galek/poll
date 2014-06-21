#include "fragmic.h"


/**
 *
 * Classes to work on:
 *
 * Timer
 * Renderable (parent object)
 * Cube (paremt object for for example Aabb)
 * Profiling / Debugging
 */

static void toggleMouseCursor(void)
{
  static bool flagMouseCursor;
  if (flagMouseCursor)
    SDL_ShowCursor(1);
  else
    SDL_ShowCursor(0);
  flagMouseCursor = !flagMouseCursor;
}


Fragmic::Fragmic(const std::string & title, const int &width,
    const int &height):
  buffers(),
  camera(width, height), 
  context(), 
  shader(), 
  scene(), 
  window(width, height),
  toggleMouseView(false),
  togglePolygonView(false)
{
  window.init(title);
  if (!context.init(window)) {
    exit(-1);
  }
  shader.load("shaders/animation.v", "shaders/animation.f");
  shader.use();
  shader.createDefaultBindings();
  context.setSwapInterval(0);

  buffers.init();
  camera.init(buffers);
  scene.init(buffers);
}


Fragmic::~Fragmic() 
{
}


void Fragmic::calcFps(Uint32 dt)
{
  static int numFrames = 0;
  static Uint32 t;

  numFrames++;
  t += dt;

  if (t >= 1000) {
    std::cout << numFrames << " frames/sec, " << 1000.0 /
      numFrames << " ms/frame" << std::endl;
    t = 0;
    numFrames = 0;
  }
}


bool Fragmic::keyboardCallbackPressed(SDL_Keysym * keysym)
{
  if (customKeyboardPressedCallback)
    customKeyboardPressedCallback(keysym);

  switch (keysym->sym) {
    case SDLK_ESCAPE:
      return false;
      break;
    case SDLK_w:
      camera.addMove(FORWARD);
      break;
    case SDLK_q:
      camera.addMove(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      camera.addMove(BACKWARD);
      break;
    case SDLK_e:
      camera.addMove(SIDESTEP_RIGHT);
      break;
    case SDLK_m:
      toggleMouseCursor();
      toggleMouseView = !toggleMouseView;
      break;
    case SDLK_p:
      context.togglePolygonMesh(togglePolygonView);
      togglePolygonView = !togglePolygonView;
      break;
    default:
      break;
  }

  return true;
}


void Fragmic::keyboardCallbackReleased(SDL_Keysym * keysym)
{

  switch (keysym->sym) {
    case SDLK_w:
      camera.deleteMove(FORWARD);
      break;
    case SDLK_q:
      camera.deleteMove(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      camera.deleteMove(BACKWARD);
      break;
    case SDLK_e:
      camera.deleteMove(SIDESTEP_RIGHT);
      break;
    default:
      break;
  }

}


void Fragmic::mouseButtonDown(SDL_MouseButtonEvent * ev)
{
  if (ev->button != 3 || !toggleMouseView)
    return;
  camera.addMove(FORWARD);
}


void Fragmic::mouseButtonUp(SDL_MouseButtonEvent * ev)
{
  if (ev->button != 3 || !toggleMouseView)
    return;
  camera.deleteMove(FORWARD);
}


void Fragmic::mouseMotion(SDL_MouseMotionEvent *ev)
{
  static int last_x, last_y;

  if (!toggleMouseView)
    return;

  if (!last_x)
    last_x = ev->x;
  if (!last_y)
    last_y = ev->y;

  camera.mouseViewUpdate(window, ev->x, ev->y);

  last_x = ev->x;
  last_y = ev->y;
}


bool Fragmic::pollEvents()
{
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
        break;
      case SDL_KEYUP:
        keyboardCallbackReleased(&event.key.keysym);
        break;
      case SDL_KEYDOWN:
        return keyboardCallbackPressed(&event.key.keysym);
      case SDL_MOUSEBUTTONDOWN:
        mouseButtonDown(&event.button);
        break;
      case SDL_MOUSEBUTTONUP:
        mouseButtonUp(&event.button);
        break;
      case SDL_MOUSEMOTION:
        mouseMotion(&event.motion);
        break;
        break;
      default:
        break;
    }
  }

  return true;
}


void Fragmic::toggleDebug()
{
  static int toggle = 0;

  toggle = !toggle;

  GLuniformBuffer *buffer = buffers.getDebugBuffer();

  std::cout << "TOGGLE: " << toggle << std::endl;

  glm::vec4 d(0.5, 0.5, 0.5, toggle);
  buffer->update(d, 0);
}


void Fragmic::run()
{
  Uint32 lastTime = SDL_GetTicks();

  for (;;) {
    Uint32 timeNow = SDL_GetTicks();
    Uint32 dt = timeNow - lastTime;
    lastTime = timeNow;

    calcFps(dt);

    if (!pollEvents()) {
      std::cout << "Fragmic exiting..." << std::endl;
      return;
    }
    
    window.checkError();

    context.clear();
    
    camera.update((double) dt / 1000.0);
    scene.update(dt);
    scene.render();

    context.checkError();
    context.swap(window);
  }

}


void Fragmic::terminate()
{
  context.term();
  window.term();
}
