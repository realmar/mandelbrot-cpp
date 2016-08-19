#include <iostream>
#include <fstream>
#include <string>

#include <vector>

// using opengl for rendering
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int  WIDTH = 512,
                    HEIGHT = 512;

void readShader(const char* file, char** shader, long& size_file) {
  std::fstream fd;
  fd.open(file, std::ios::in | std::ios::ate);
  size_file = fd.tellg();
  fd.seekg(0, std::ios::beg);
  *shader = new char[size_file];
  fd.read(*shader, size_file);
  fd.close();
}

GLuint loadShader(const char *fragment_path) {
  // define variables
  char *frag_shader;
  long frag_s_len;

  // load shader files into memory
  readShader(fragment_path, &frag_shader, frag_s_len);

  int frag_s_len_int =(int)frag_s_len;

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

int main(int argc, char** argv) {
  if(!glfwInit()) {
    std::cout << "glfw is not so ok" << std::endl;
    exit(1);
  }

  // glewExperimental=GL_TRUE;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GPU mandelbrot", NULL, NULL);
  if(!window) {
    std::cout << "open window glfw is not so ok" << std::endl;
    exit(1);
  }

  glfwMakeContextCurrent(window);

  // init glew
  glewExperimental = GL_TRUE;
  GLenum glew_init = glewInit();
  if(glew_init != GLEW_OK) {
    std::cout << "glew is not so ok: " << glewGetErrorString(glew_init) << std::endl;
    exit(1);
  }

  GLuint program = loadShader("shader.frag");
  std::cout << "shader loaded" << std::endl;

   float vertices[] = {
      -1.0f, 1.0f,
      -1.0f, -1.0f,
      1.0f, -1.0f,

      -1.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, -1.0f
    };

  std::cout << "vertices generated" << std::endl;

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  while(!glfwWindowShouldClose(window)) {
    glViewport(0, 0, WIDTH, HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // close context
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}