#ifndef VERTEX_OBJECT_H
#define VERTEX_OBJECT_H

#include <GL/glew.h>

class VertexObject {
  private:
    GLuint vbo;
    GLuint vao;
    GLuint program;
    unsigned int size;
    unsigned int dimensions;
    void rebuildMemoryLayout(const float* vertices);
  public:
    VertexObject(const char* shader_path, const float* vertices, const unsigned int& size, const unsigned int& dimensions);
    GLuint& getVertexAttribute(void);
    GLuint& getProgram(void);
    void setVertices(const float* vertices);
};

#endif
