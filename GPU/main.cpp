#include <iostream>   // IO
#include <fstream>    // to load shaders from file

#include <vector>     // Used for storing the opengl error messages

// using opengl for rendering

/*
What is glew?

GLEW isn't 'required'. You could use glcorearb.h header, or anything like that. However, if you link with some function - it must exist on target platform, or your program wouldn't launch. GLEW and others are exactly to handle that - you're not linking with GL functions directly, but instead getting function pointers after initialization phase. It allows you to check at runtime which extensions are present and which functions may be used.
*/
#include <GL/glew.h>
/*
What is glfw?

GLFW is a small C library that allows the creation and management of windows with OpenGL contexts, making it also possible to use multiple monitors and video modes. It provides access to input from keyboard, mouse and joysticks.
*/
#include <GLFW/glfw3.h>

const unsigned int  WIDTH = 1024,
                    HEIGHT = 1024;

void readShader(const char* file, char** shader, long& size_file) {
  std::fstream fd;
  fd.open(file, std::ios::in | std::ios::ate);
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

  // we require at least opengl 4
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   // window should no be resizable

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

  // load the fragment shader
  GLuint program = loadShader("shader.frag");
  std::cout << "shader loaded" << std::endl;

  // those two vertices fill the whole window
  // where (0, 0) is in the middle of the screen
   float vertices[] = {
      -1.0f, 1.0f,
      -1.0f, -1.0f,
      1.0f, -1.0f,

      -1.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, -1.0f
    };

  std::cout << "vertices generated" << std::endl;

  // create vertex buffer object
  //
  // we essentially create an empty buffer and replace the current with it
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

  // create vertex attribute object
  //
  // store the memory layout of the mesh (our vertices) so that we
  // dont have to rebuilt it every time we want to draw it
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  // get unifrom variables from shader
  GLint uni_width = glGetUniformLocation(program, "width");
  GLint uni_height = glGetUniformLocation(program, "height");


  while(!glfwWindowShouldClose(window)) {
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    // setting uniforms
    glUniform1f(uni_width, (float)WIDTH);
    glUniform1f(uni_height, (float)HEIGHT);

    // draw stuff
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // listen to external user input eg mouse keyboard
    glfwPollEvents();

    // replace frame buffer
    glfwSwapBuffers(window);
  }

  // close context
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
