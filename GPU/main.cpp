#include <iostream>   // IO

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
#include "shader_util.hpp"

const unsigned int  WIDTH = 1024,
                    HEIGHT = 1024;

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

    // draw stuff using our vertex attribute object
    // which holds the memory layout of our mesh
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
