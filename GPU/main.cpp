#include <iostream>   // IO
#include <algorithm>  // for max and min functions

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

float x_map[2] = {-2.5, 1};
float y_map[2] = {-1, 1};

struct Uniforms {
  GLint uni_width;
  GLint uni_height;

  GLint uni_x_map;
  GLint uni_y_map;
};

Uniforms uniforms;

struct MousePosition {
  double x;
  double y;
};

MousePosition mouse_position;
bool get_last_mouse_pos = false;
bool get_curr_mouse_pos = false;

float mapPixel(const float& p, const float* map, const float& orig_Width) {
  return p / orig_Width * abs(map[0] - map[1]) + map[0];
}

void mouseClick(GLFWwindow* window, int button, int action, int mods) {
  if(button == GLFW_MOUSE_BUTTON_LEFT) {
    if(action == GLFW_PRESS) {
      get_last_mouse_pos = true;
    }else if(action == GLFW_RELEASE) {
      get_curr_mouse_pos = true;
    }
  }
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
  if(get_last_mouse_pos) {
    get_last_mouse_pos = false;

    mouse_position.x = xpos;
    mouse_position.y = ypos;
  }

  if(get_curr_mouse_pos) {
    get_curr_mouse_pos = false;

    float x_map_tmp[2];
    float y_map_tmp[2];

    std::cout << mouse_position.x << " : " << mouse_position.y << std::endl;
    std::cout << xpos << " : " << ypos << std::endl;

    x_map_tmp[0] = mapPixel(std::min(mouse_position.x, xpos), x_map, WIDTH);
    x_map_tmp[1] = mapPixel(std::max(mouse_position.x, xpos), x_map, WIDTH);

    y_map_tmp[0] = y_map[0] * (x_map_tmp[0] / x_map[0]);
    y_map_tmp[1] = y_map[1] * (x_map_tmp[1] / x_map[1]);

    std::cout << x_map_tmp[0] << " : " << x_map_tmp[1] << " :: " << y_map_tmp[0] << " : " << y_map_tmp[1] << std::endl;

    x_map[0] = x_map_tmp[0];
    x_map[1] = x_map_tmp[1];

    y_map[0] = y_map_tmp[0];
    y_map[1] = y_map_tmp[1];
  }
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

  uniforms.uni_width = glGetUniformLocation(program, "width");
  uniforms.uni_height = glGetUniformLocation(program, "height");

  uniforms.uni_x_map = glGetUniformLocation(program, "x_map");
  uniforms.uni_y_map = glGetUniformLocation(program, "y_map");

  // set mouse callback to register click input
  glfwSetMouseButtonCallback(window, mouseClick);
  glfwSetCursorPosCallback(window, mouseMove);

  while(!glfwWindowShouldClose(window)) {
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    // setting uniforms
    glUniform1f(uniforms.uni_width, (float)WIDTH);
    glUniform1f(uniforms.uni_height, (float)HEIGHT);

    // set the mapping range for the complex plane
    glUniform2f(uniforms.uni_x_map, x_map[0], x_map[1]);
    glUniform2f(uniforms.uni_y_map, y_map[0], y_map[1]);

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
