#include <GL/glew.h>
#include "shader_util.hpp"
#include "vertex_object.hpp"

VertexObject::VertexObject(const char* shader_path, const float* vertices, const unsigned int& size, const unsigned int& dimensions) {
  // load the fragment shader
  program = loadShader(shader_path);
  this->size = size;
  this->dimensions = dimensions;

  rebuildMemoryLayout(vertices);
}

void VertexObject::setVertices(const float* vertices) {
  rebuildMemoryLayout(vertices);
}

GLuint& VertexObject::getVertexAttribute(void) {
  return vao;
}

GLuint& VertexObject::getProgram(void) {
  return program;
}

void VertexObject::rebuildMemoryLayout(const float* vertices) {
  // create vertex buffer object
  //
  // we essentially create an empty buffer and replace the current with it
  vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, size * dimensions * sizeof(float), vertices, GL_STATIC_DRAW);

  // create vertex attribute object
  //
  // store the memory layout of the mesh (our vertices) so that we
  // dont have to rebuilt it every time we want to draw it
  vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}
