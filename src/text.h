#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include "mesh.h"
#include "texture.h"


class GLcontext;


class Font
{
  private:
    unsigned char *buffer;
    Texture texture;

  public:
    Font();
    ~Font();

    void     bake();
    void     load(const std::string &filename);
    Texture &texture_get();
};



class Text
{
  private:
    Font *font;
    std::string input;

  public:

    Text();
    ~Text();

    void          bake(GLcontext *glcontext, Mesh *mesh, float x, float y);
    Font         *font_get();
    void          font_set(Font *font);
    void          string_append(const std::string s);
    void          string_erase_last();
    std::string  &string_get();
    unsigned int  string_len();
    void          string_set(std::string input);

};
