#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <stdexcept>

#include "camera.h"
#include "gldefaults.h"
#include "glshader.h"
#include "scene.h"


class GLcontext {
  private:
    GLuint gl_buffer_globalmatrices;
    GLuint gl_buffer_matrices;
    GLuint gl_buffer_armature;
    GLuint gl_buffer_material;
    GLuint gl_buffer_state;
    GLuint gl_buffer_light;
    GLuint gl_vertex_buffers[8];
    GLuint gl_uniform_camera_pos;

    GLuint gl_fb_tex;
    GLuint gl_fb;
    GLuint gl_fb_vertex_buffers[1];

    Node *fb_node;

    bool check_version(const int &major);
    void texture_cubemap_create(Cubemap_Item &item);
    void texture_cubemap_delete(Cubemap &cubemap);
    void texture_create(Texture &texture, GLenum n);
    void texture_delete(Texture &texture);

  public:
    GLcontext();
    ~GLcontext();

    void check_error();
    void clear();
    void node_draw(Node &node);
    void framebuffer_create(const int width, const int height);
    void framebuffer_node_create(GLshader &shader, Node &node);
    void framebuffer_draw_texture(Scene &scene, bool debug);
    void framebuffer_draw_screen();
    void framebuffer_delete();
    bool init(const int width, const int height);
    void polygon_mesh_toggle(bool tog);
    void uniform_buffers_create(GLshader &shader);
    void uniform_buffers_delete();
    void uniform_buffers_update_camera(Camera &camera);
    void uniform_buffers_update_light_num(const unsigned int num_lights);
    void uniform_buffers_update_light2(const Light &light, const unsigned int index);
    void uniform_buffers_update_light(const std::vector<Light_Properties *> &light_properties);
    void uniform_buffers_update_material(Material &material);
    void uniform_buffers_update_mesh(Mesh &mesh);
    void uniform_buffers_update_node(Node &node);
    void uniform_buffers_update_state(Node &node);
    void vertex_buffers_create(Node &node);
    void vertex_buffers_delete(Node &node);
};
