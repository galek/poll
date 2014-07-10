#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "animated.h"
#include "gldefaults.h"
#include "utils.h"


enum CameraMove { FORWARD, BACKWARD, SIDESTEP_RIGHT, SIDESTEP_LEFT};

class CameraPan:public Animated {
  public:
    bool toggle;
    glm::vec3 position;

    glm::vec3 getInterpolatedTranslation(double factor) 
    {
      glm::vec3 tInterp;
      int prevFrame = this->keyframe_prev_get();
      int nextFrame = this->keyframe_next_get();
      if (keyframes[prevFrame]->t != keyframes[nextFrame]->t) {
        tInterp =
          glm::mix(keyframes[prevFrame]->t, keyframes[nextFrame]->t,
              (float) factor);
      } else {
        tInterp = keyframes[prevFrame]->t;
      }
      return tInterp;
    }

    bool stepTime(double dt) 
    {
      
      if (keyframes.size() == 0)
        return false;
      
      animation_time = animation_time + dt;
      if (!step(animation_time)) {
        return false;
      }

      double factor = step_factor_get(animation_time);
      position = getInterpolatedTranslation(factor);
      return true;
    }
};

class Camera {
  private:
    float fov;
    float initial_fov;
    float horizontalAngle;
    float verticalAngle;
    float speed;
    float mouseSpeed;
    glm::vec3 up;
    glm::vec3 direction;
    glm::vec3 right;
    std::vector < CameraMove > cameraMoveQueue;
    CameraPan cameraPan;

    void updateViewProjection(void);
    void updateProjection(void);
    bool processInputMoves(void);
    void processWayPoints(double dt);

  public: 

    Camera(int width, int height);
    ~Camera(void);
    glm::mat4 perspective;
    glm::mat4 ortho;
    glm::mat4 view;
    glm::vec3 position;
    glm::vec3 target;
    glm::mat4 modelView;
    glm::mat4 viewProjection;
    void addMove(CameraMove move);
    void deleteMove(CameraMove move);
    void update(double dt);
    void computeDirections();
    void updateView();
    bool mouseViewUpdate(int xPos, int yPos, int width, int height);
    void setWayPoints(std::vector < glm::vec3 > points,
        std::vector < Uint32 > time, glm::vec3 _target,
        bool useCurPos);
    void setStartPosition(void);
    void createPerspectiveProjection(float width, float height);
    void createOrthoProjection(float width, float height);
    void translate(glm::vec3 dir);
};
