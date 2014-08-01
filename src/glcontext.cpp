#include "glcontext.h"
#include "material.h"
#include "node.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


GLcontext::GLcontext()
{
}


GLcontext::~GLcontext()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void GLcontext::check_error()
{
  GLenum gl_error = glGetError();
  if (GL_NO_ERROR != gl_error) {
    std::cout << "OpenGL error: " << gluErrorString(gl_error) << std::endl;   
  }
}


void GLcontext::clear()
{
  glm::vec4 color(0.5, 0.5, 0.5, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(color.x, color.y, color.z, color.a);
}


bool GLcontext::init(const int width, const int height)
{
  glewExperimental= GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "GLcontext ERROR: failed to initalize GLEW" << std::endl;
    return false;
  }
  check_error();
  check_version(3);

  glViewport(0, 0, width, height);
  //GL_ASSERT(glEnable(GL_CULL_FACE)); // cull face
  //GL_ASSERT(glCullFace(GL_BACK)); // cull back face
  //GL_ASSERT(glFrontFace(GL_CCW)); // GL_CCW for counter clock-wise


  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  return true;
}


void GLcontext::draw_light(Light *light)
{
  Mesh *mesh = light->volume_mesh_get();
  if (!mesh) {
    return;
  }

  if (light->node_follow_get()) {
    Mesh *follow_mesh = light->node_follow_get()->mesh_get();
    glm::vec4 new_pos = follow_mesh->model *  glm::vec4(light->node_follow_get()->original_position, 1);
    light->properties_position_set(glm::vec3(new_pos));

    uniform_buffers_update_mesh(*follow_mesh);
  } else {
    uniform_buffers_update_mesh(*mesh);
  }

  uniform_buffers_update_light(*light);
  draw_mesh(*mesh);

}

void GLcontext::draw_node(Node &node)
{
  Mesh *mesh = node.mesh;

  if (!mesh) {
    std::cout << "No mesh attached to node: '" << node.name << std::endl;
    return;
  }

    uniform_buffers_update_state(node);
  {
    uniform_buffers_update_mesh(*mesh);
  }

  { 
    Material *material = node.material_get();
    if (material) uniform_buffers_update_material(*material);
  }

  if (node.state.cubemap_skybox) glDepthMask(GL_FALSE);
  draw_mesh(*mesh);
  if (node.state.cubemap_skybox) glDepthMask(GL_TRUE);

}


void GLcontext::draw_mesh(Mesh &mesh)
{
  GL_ASSERT(glBindVertexArray(mesh.gl_vao));
  GLsizei count = (GLsizei) mesh.num_indices_get();
  if (count <= 0) {
    GL_ASSERT(glDrawArrays(mesh.mode, 0, mesh.num_vertices_get()));
  } else {
    GL_ASSERT(glDrawElements(mesh.mode, count, GL_UNSIGNED_SHORT, 0));
  }
}


void GLcontext::framebuffer_check_status() 
{
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (GL_FRAMEBUFFER_COMPLETE != status) {
    fprintf (stderr, "ERROR: incomplete framebuffer\n");
    if (GL_FRAMEBUFFER_UNDEFINED == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_UNDEFINED\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER== status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
    } else if (GL_FRAMEBUFFER_UNSUPPORTED == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_UNSUPPORTED\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n");
    } else {
      fprintf (stderr, "unspecified error\n");
    }
    exit(-1);
  }
}


void GLcontext::framebuffer_delete()
{

}


void GLcontext::framebuffer_g_create(GLshader &glshader_deferred_second, const int width, const int height)
{
  GLuint program = glshader_deferred_second.program;

  glGenTextures(1, &gl_g_fb_tex_normal);
  glBindTexture(GL_TEXTURE_2D, gl_g_fb_tex_normal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glGenTextures(1, &gl_g_fb_tex_diffuse);
  glBindTexture(GL_TEXTURE_2D, gl_g_fb_tex_diffuse);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glGenFramebuffers (1, &gl_g_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, gl_g_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_g_fb_tex_normal, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gl_g_fb_tex_diffuse, 0);
  GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, draw_bufs);

  framebuffer_check_status();

  glGenTextures(1, &gl_g_fb_tex_depth);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gl_g_fb_tex_depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gl_g_fb_tex_depth, 0);

  glGenTextures(1, &gl_g_fb_tex_final);
  glBindTexture(GL_TEXTURE_2D, gl_g_fb_tex_final);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gl_g_fb_tex_final, 0);


  glshader_deferred_second.use();
  GLint location;
  location = glGetUniformLocation(program, "normal_tex");
  GL_ASSERT(glUniform1i(location, 0));
  location = glGetUniformLocation(program, "diffuse_tex");
  GL_ASSERT(glUniform1i(location, 1));
  location = glGetUniformLocation(program, "depth_tex");
  GL_ASSERT(glUniform1i(location, 2));
}


void GLcontext::framebuffer_g_draw_geometry(Scene &scene, GLshader &shader_first_pass)
{


}


void GLcontext::framebuffer_g_light_pass(GLshader &shader_light, Light &light)
{
}


void GLcontext::framebuffer_g_draw_illuminated_scene(const Assets &assets, Scene &scene, GLshader shader_geometry,  GLshader &shader_stencil, GLshader &shader_light)
{
  auto &lights = assets.light_active_get();

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_g_fb);
  glDrawBuffer(GL_COLOR_ATTACHMENT2);
  glClear(GL_COLOR_BUFFER_BIT);
  
  /* GEOMETRY PASS */
  shader_geometry.use();
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_g_fb);
  GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, draw_bufs);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  for (auto &node: scene.mesh_nodes_get()) {
    draw_node(*node);
  }
  glDepthMask(GL_FALSE);

  /* STENCIL AND LIGHT PASS*/
  glEnable(GL_STENCIL_TEST);
  for (auto &light: lights) {
    if (!light->volume_mesh_get()) 
      continue;

    /* STENCIL PASS */
    shader_stencil.use();
    glDrawBuffer(GL_NONE);
    glClear(GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glStencilFunc(GL_ALWAYS, 0, 0);

    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    draw_light(light.get());

    /* LIGHT PASS */
    shader_light.use();
    glDrawBuffer(GL_COLOR_ATTACHMENT2);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_g_fb_tex_normal);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gl_g_fb_tex_diffuse);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gl_g_fb_tex_depth);

    draw_light(light.get());

    glCullFace(GL_BACK);
  }
  glDisable(GL_STENCIL_TEST);

  /***** BLIT FINAL FRAME *********/
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gl_g_fb);
  glReadBuffer(GL_COLOR_ATTACHMENT2);
  GL_ASSERT(glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST));
}


void GLcontext::framebuffer_g_stencil_pass(GLshader &shader_stencil, Light &light)
{

}


void GLcontext::framebuffer_node_create(Node &node)
{
  GLenum target;
  GLint index;
  Mesh *mesh = node.mesh;

  glGenVertexArrays(1, &mesh->gl_vao);
  glBindVertexArray(mesh->gl_vao);
  glGenBuffers(1, gl_fb_vertex_buffers);

  target = GL_ARRAY_BUFFER;
  {
    std::vector<glm::vec3> positions = mesh->positions_get();
    index = 0;
    glBindBuffer(target, gl_fb_vertex_buffers[index]);
    glBufferData(target, positions.size() * sizeof(positions[0]), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  fb_node = &node;
}


void GLcontext::polygon_mesh_toggle(bool tog)
{
  glPolygonMode(GL_FRONT_AND_BACK, tog ? GL_FILL : GL_LINE);
}


void GLcontext::uniform_buffers_block_bind(GLshader &shader)
{
  GLuint program = shader.program;
  GLuint block_index;
  GLuint bind_index;

  for (auto &name : shader.block_names_get()) {
    //   std::cout << name << " bind index: " << uniform_buffer_map.at(name) << std::endl;
    bind_index = uniform_buffer_map.at(name);
    block_index = shader.get_block_index(name);
    GL_ASSERT(glUniformBlockBinding(program, block_index, bind_index));
  }
}


void GLcontext::uniform_buffers_create()
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLuint bind_index;

  {
    glm::mat4 matrix[3];
    bind_index = UB_GLOBALMATRICES;
    glGenBuffers(1, &gl_buffer_globalmatrices);
    glBindBuffer(target, gl_buffer_globalmatrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferBase(target, bind_index, gl_buffer_globalmatrices);
    glBindBuffer(target, 0);
  }

  {
    glm::mat4 matrix;
    bind_index = UB_MATRICES;
    glGenBuffers(1, &gl_buffer_matrices);
    glBindBuffer(target, gl_buffer_matrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferBase(target, bind_index, gl_buffer_matrices);
    glBindBuffer(target, 0);
  }

  {
    glm::mat4 matrix[64];
    bind_index = UB_ARMATURE;
    glGenBuffers(1, &gl_buffer_armature);
    glBindBuffer(target, gl_buffer_armature);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferBase(target, bind_index, gl_buffer_armature);
    glBindBuffer(target, 0);
  }

  {
    Material_Properties properties;
    properties.Ka = glm::vec4(1.f, 0.f, 0.f, 1.f);
    properties.Kd = glm::vec4(0.f, 1.f, 0.f, 1.f);
    properties.Ks = glm::vec4(0.f, 0.f, 1.f, 1.f);
    properties.shininess = 0.f;

    bind_index = UB_MATERIAL;
    glGenBuffers(1, &gl_buffer_material);
    glBindBuffer(target, gl_buffer_material);
    glBufferData(target, sizeof(properties), &properties, GL_STREAM_DRAW);
    glBindBufferBase(target, bind_index, gl_buffer_material);
    glBindBuffer(target, 0);
  }

  {
    Node_State state;
    state.animated = false;
    state.debug = false;
    state.diffuse = false;
    state.diffuse_normal = false;
    state.diffuse_specular_normal = false;
    state.cubemap_reflect = false;
    state.cubemap_skybox = false;
    state.standard = false;

    bind_index = UB_STATE;
    glGenBuffers(1, &gl_buffer_state);
    glBindBuffer(target, gl_buffer_state);
    glBufferData(target, sizeof(state), &state, GL_STREAM_DRAW);
    glBindBufferBase(target, bind_index, gl_buffer_state);
    glBindBuffer(target, 0);
  }

  {
    Light_Properties properties;

    bind_index = UB_LIGHT;
    glGenBuffers(1, &gl_buffer_light);
    glBindBuffer(target, gl_buffer_light);
    glBufferData(target, sizeof(properties), &properties, GL_STREAM_DRAW);
    glBindBufferBase(target, bind_index, gl_buffer_light);
    glBindBuffer(target, 0);
  }
}


void GLcontext::uniform_buffers_delete()
{
  glDeleteBuffers(1, &gl_buffer_globalmatrices);
  glDeleteBuffers(1, &gl_buffer_matrices);
  glDeleteBuffers(1, &gl_buffer_armature);
  glDeleteBuffers(1, &gl_buffer_material);
  glDeleteBuffers(1, &gl_buffer_state);
}


void GLcontext::uniform_buffers_update_armature(const Armature &armature)
{
  GLenum target = GL_UNIFORM_BUFFER;
  glBindBuffer(target, gl_buffer_armature);
  glBufferSubData(target, 0, armature.skinning_matrices.size() * sizeof(armature.skinning_matrices[0]), 
      armature.skinning_matrices.data());
  glBindBuffer(target, 0);
}


void GLcontext::uniform_buffers_update_camera(Camera &camera)
{
  glm::mat4 data[3];
  data[0] = camera.transform_perspective_get();
  data[1] = camera.transform_perspective_inverse_get();
  data[2] = camera.transform_view_get();
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_globalmatrices));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_buffers_update_light(Light &light)
{
  Light_Properties properties = light.properties_get();

  //std::cout << glm::to_string(properties.position) << std::endl;
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_light));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(properties), &properties));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_buffers_update_material(const Material &material)
{
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_material));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(material.properties), &material.properties));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0));

  if (material.diffuse) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuse->gl_texture);
  }
  if (material.normal) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.normal->gl_texture);
  }
  if (material.specular) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material.specular->gl_texture);
  }
  if (material.cubemap) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, material.cubemap->gl_texture);
  }
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void GLcontext::uniform_buffers_update_mesh(Mesh &mesh)
{
  glm::mat4 m;
  m = mesh.model;

  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_matrices));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m), &m));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_buffers_update_state(Node &node)
{
  /*
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_state));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(node.state), &node.state));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
  */
}


void GLcontext::uniform_textures_init(GLshader &shader)
{
  GLuint program = shader.program;

  GLint location;
  location = glGetUniformLocation(program, "diffuse_texture");
  GL_ASSERT(glUniform1i(location, 0));
  location = glGetUniformLocation(program, "normal_texture");
  GL_ASSERT(glUniform1i(location, 1));
  location = glGetUniformLocation(program, "specular_texture");
  GL_ASSERT(glUniform1i(location, 2));
}


void GLcontext::vertex_buffers_mesh_create(Mesh *mesh)
{
  GLenum target;
  GLint index;

  if (!mesh) return;

  if (glIsVertexArray(mesh->gl_vao)) {
    std::cout << "Error: vertex array already exists for the mesh" << std::endl;
    return;
  }

  GL_ASSERT(glGenVertexArrays(1, &mesh->gl_vao));
  GL_ASSERT(glBindVertexArray(mesh->gl_vao));
  GL_ASSERT(glGenBuffers(8, gl_vertex_buffers));

  target = GL_ARRAY_BUFFER;
  {
    std::vector<glm::vec3> positions = mesh->positions_get();
    index = 0;
    GL_ASSERT(glBindBuffer(target, gl_vertex_buffers[index]));
    glBufferData(target, positions.size() * sizeof(positions[0]), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  {
    std::vector<glm::vec3> normals = mesh->normals_get();
    index = 1;
    if (normals.size() > 0) {
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::vec3> tangents = mesh->tangents_get();
    if (tangents.size() > 0) {
      index = 2;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, tangents.size() * sizeof(tangents[0]), tangents.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::vec3> bitangents = mesh->bitangents_get();
    if (bitangents.size() > 0) {
      index = 3;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, bitangents.size() * sizeof(bitangents[0]), bitangents.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::vec3> bone_weights = mesh->bone_weights_get();
    if (bone_weights.size() > 0) {
      index = 4;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, bone_weights.size() * sizeof(bone_weights[0]), bone_weights.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::ivec3> bone_indices = mesh->bone_indices_get();
    if (bone_indices.size() > 0) {
      index = 5;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, bone_indices.size() * sizeof(bone_indices[0]), bone_indices.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribIPointer(index, 3, GL_INT, 0, 0);
    }
  }

  {
    std::vector<glm::vec2> uvs = mesh->texture_st_get();
    if (uvs.size() > 0) {
      index = 6;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, uvs.size() * sizeof(uvs[0]), uvs.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    target = GL_ELEMENT_ARRAY_BUFFER;
    std::vector<GLshort> indices = mesh->indices_get();
    glBindBuffer(target, gl_vertex_buffers[7]);
    glBufferData(target, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
  }

}


void GLcontext::vertex_buffers_create(Node &node)
{
  Mesh *mesh = node.mesh;
  Material *material = node.material;

  if (!mesh) return;
  vertex_buffers_mesh_create(mesh);

  if (material) {
    if (material->diffuse) {
      texture_create(*material->diffuse, GL_TEXTURE0);
    }
    if (material->normal) {
      texture_create(*material->normal, GL_TEXTURE1);
    }
    if (material->specular) {
      texture_create(*material->specular, GL_TEXTURE2);
    }
    if (material->cubemap) {
      if (!glIsTexture(material->cubemap->gl_texture)) {
        GL_ASSERT(glGenTextures(1,  &material->cubemap->gl_texture));
        GL_ASSERT(glActiveTexture(GL_TEXTURE3));
        GL_ASSERT(glBindTexture(GL_TEXTURE_CUBE_MAP, material->cubemap->gl_texture));
        texture_cubemap_create(material->cubemap->front);
        texture_cubemap_create(material->cubemap->back);
        texture_cubemap_create(material->cubemap->top);
        texture_cubemap_create(material->cubemap->bottom);
        texture_cubemap_create(material->cubemap->left);
        texture_cubemap_create(material->cubemap->right);
      }
    }
  }
}


void GLcontext::vertex_buffers_delete(Node &node)
{
  Mesh *mesh = node.mesh;
  Material *material = node.material;

  if (mesh) {
    GL_ASSERT(glDeleteBuffers(8, gl_vertex_buffers));
    GL_ASSERT(glDeleteVertexArrays(1, &mesh->gl_vao));
  }
  if (material) {
    if (material->diffuse) {
      texture_delete(*material->diffuse);
    }
    if (material->normal) {
      texture_delete(*material->normal);
    }
    if (material->specular) {
      texture_delete(*material->specular);
    }
    if (material->cubemap) {
      texture_cubemap_delete(*material->cubemap);
    }
  }

}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


bool GLcontext::check_version(const int &major)
{
  int maj, min;

  glGetIntegerv(GL_MAJOR_VERSION, &maj);
  glGetIntegerv(GL_MINOR_VERSION, &min);

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL version number: " << maj << "." << min << std::endl;
  std::cout << "GLSL version number: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "GL vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "GL renderer: " << glGetString(GL_RENDERER) << std::endl;


  if (maj < major)
    return false;

  return true;
}


void GLcontext::texture_create(Texture &texture, GLenum n) 
{
  if (glIsTexture(texture.gl_texture)) {
    return;
  }

  glGenTextures(1, &texture.gl_texture);
  glActiveTexture(n);
  glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.image->width, texture.image->height,
      0, GL_RGB, GL_UNSIGNED_BYTE, texture.image->data);
}


void GLcontext::texture_cubemap_create(Cubemap_Item &item) 
{
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(item.target, 0, GL_RGB, item.texture.image->width, item.texture.image->height,
      0, GL_RGB, GL_UNSIGNED_BYTE, item.texture.image->data);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void GLcontext::texture_cubemap_delete(Cubemap &cubemap)
{
  if (glIsTexture(cubemap.gl_texture)) {
    std::cout << "Deleting opengl cubemap texture buffer" << std::endl;
    glDeleteTextures(1, &cubemap.gl_texture);
  }
}


void GLcontext::texture_delete(Texture &texture)
{
  if (glIsTexture(texture.gl_texture)) {
    std::cout << "Deleting opengl texture buffer: " << texture.filename << "'" <<  std::endl;
    glDeleteTextures(1, &texture.gl_texture);
  } 
}


