#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>    // to load shaders from file
#include <vector>     // Used for storing the opengl error messages

void readShader(const char* file, char** shader, long& size_file) {
  std::fstream fd;
  fd.open(file, std::ios::in | std::ios::binary | std::ios::ate);
  size_file = fd.tellg();
  fd.seekg(0, std::ios::beg);
  *shader = new char[size_file];
  fd.read(*shader, size_file);
  fd.close();
}

/*
We only need a fragment shader because we do not have any vertices which needs
to be placed on the screen.

A fragment shader (pixel shader) is a shader which determines what color
a pixel has.
*/
GLuint loadShader(const char *fragment_path) {
  // define variables
  char *frag_shader;
  long frag_s_len;

  // load shader files into memory
  readShader(fragment_path, &frag_shader, frag_s_len);

  std::cout << frag_shader << std::endl;

  int frag_s_len_int = (int)frag_s_len;

  // This stuff has to be const ...
  const char* frag_shader_c =(const char*)frag_shader;

  std::cout << "read shader in memory" << std::endl;

  // create vertex and fragment shader objects
  GLuint fragment_shader_object = glCreateShader(GL_FRAGMENT_SHADER);

  std::cout << "created shader object: " << std::endl;

  // attach shader source to shader objects
  glShaderSource(fragment_shader_object, 1, &frag_shader_c, &frag_s_len_int);

  std::cout << "attached shader to shader object" << std::endl;

  // compile the shaders
  glCompileShader(fragment_shader_object);

  std::cout << "compiled shader" << std::endl;

  int log_length;

  // verify that shaders are corectly compild
  GLint frag_compiled;
  glGetShaderiv(fragment_shader_object, GL_COMPILE_STATUS, &frag_compiled);
  if(!frag_compiled/* || !vert_compiled*/) {

    glGetShaderiv(fragment_shader_object, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> frag_shader_error((log_length > 1) ? log_length : 1);
    glGetShaderInfoLog(fragment_shader_object, log_length, NULL, &frag_shader_error[0]);
    std::cout << &frag_shader_error[0] << std::endl;

    std::cout << "Shader compilation error" << std::endl;
    exit(1);
  }

  // create program and link shaders
  GLuint program_object = glCreateProgram();
  glAttachShader(program_object, fragment_shader_object);
  glLinkProgram(program_object);

  // check if sucessfully linked
  GLint linked;
  glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
  if(!linked) {
    glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> program_error((log_length > 1) ? log_length : 1 );
    glGetProgramInfoLog(program_object, log_length, NULL, &program_error[0]);
    std::cout << &program_error[0] << std::endl;

    std::cout << "Shader linking error" << std::endl;
    exit(1);
  }

  glDeleteShader(fragment_shader_object);
  delete frag_shader_c;

  return program_object;
}
