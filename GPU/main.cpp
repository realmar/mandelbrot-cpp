#include <iostream>   // IO
#include <algorithm>  // for max, min and for_each functions
#include <cmath>      // for std::fabs
#include <vector>     // for the render functions
#include <random>     // for color generation

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
#include "vertex_object.hpp"

const double  WIDTH = 1024,
              HEIGHT = 1024;

double orig_x_map[2] = {-3, 1};
double orig_y_map[2] = {-2, 2};

double x_map[2] = {-3, 1};
double y_map[2] = {-2, 2};
double screen_map[] = {-1, 1};

struct Uniforms {
  GLint uni_width;
  GLint uni_height;

  GLint uni_x_map;
  GLint uni_y_map;

  GLint uni_start_color;
  GLint uni_end_color;
};

Uniforms uniforms;

struct MousePosition {
  double x;
  double y;
};

struct Color {
  float x;
  float y;
  float z;
  float w;

  Color(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
};

// store the last mouse position
MousePosition last_mouse_position;
bool get_last_mouse_pos = false;
bool get_curr_mouse_pos = false;
bool mouse_down = false;

bool render_frame = true;

// This is the rect which gets drawn when the user points and drags with the mouse
float rect[2 * 6] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// vbo, vao, shader and program of the selection rect
VertexObject* selection_rect_vertex_o;

// Setting the initial colors
Color start_color(0.12344, 0.346532, 0.22344, 0.15563);
Color end_color(0.92344, 0.646532, 0.82344, 0.85563);

// initialize the mersenne twister algorithm for high quality random numbers
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> random01(0, 1);

// conversion between different axis
double mapPixel(const double& p, const double* map, const double& orig_Width) {
  return p / orig_Width * std::fabs(map[0] - map[1]) + map[0];
}

// mouse click callback
void mouseClick(GLFWwindow* window, int button, int action, int mods) {
  if(button == GLFW_MOUSE_BUTTON_LEFT) {
    if(action == GLFW_PRESS) {
      get_last_mouse_pos = true;
      mouse_down = true;
    }else if(action == GLFW_RELEASE) {
      get_curr_mouse_pos = true;
      mouse_down = false;
    }
  }
}

// mouse move callback
void mouseMove(GLFWwindow* window, double xpos, double ypos) {
  if(get_last_mouse_pos) {
    get_last_mouse_pos = false;

    last_mouse_position.x = xpos;
    last_mouse_position.y = ypos;
  }

  if(mouse_down) {
    float mapped_last_x_pos = mapPixel(last_mouse_position.x, screen_map, WIDTH);
    float mapped_x_pos = mapPixel(xpos, screen_map, WIDTH);

    float scale = (last_mouse_position.x - xpos) / WIDTH;

    float mapped_last_y_pos = 0 - mapPixel(last_mouse_position.y, screen_map, HEIGHT);

    float unscaled_mapped_y_pos = 0 - mapPixel(ypos, screen_map, HEIGHT);
    float direction = 1;
    if( (unscaled_mapped_y_pos < mapped_last_y_pos && mapped_x_pos < mapped_last_x_pos) ||
        (unscaled_mapped_y_pos > mapped_last_y_pos && mapped_x_pos > mapped_last_x_pos))
      direction = -1;

    float mapped_y_pos = mapped_last_y_pos + direction * (2 * scale);

    float new_rect[2 * 6] = {
      mapped_last_x_pos, mapped_last_y_pos,
      mapped_last_x_pos, mapped_y_pos,
      mapped_x_pos, mapped_y_pos,

      mapped_x_pos, mapped_last_y_pos,
      mapped_last_x_pos, mapped_last_y_pos,
      mapped_x_pos, mapped_y_pos
    };

    selection_rect_vertex_o->setVertices(new_rect);
  }

  if(get_curr_mouse_pos) {
    get_curr_mouse_pos = false;

    double x_map_tmp[2];
    double y_map_tmp[2];

    x_map_tmp[0] = mapPixel(std::min(last_mouse_position.x, xpos), x_map, WIDTH);
    x_map_tmp[1] = mapPixel(std::max(last_mouse_position.x, xpos), x_map, WIDTH);

    double scale = std::fabs(last_mouse_position.x - xpos) / WIDTH;
    double new_y_range = std::fabs(y_map[0] - y_map[1]) * scale;

    y_map_tmp[0] = mapPixel(HEIGHT - std::max(last_mouse_position.y, ypos), y_map, HEIGHT);
    y_map_tmp[1] = y_map_tmp[0] + new_y_range;

    x_map[0] = x_map_tmp[0];
    x_map[1] = x_map_tmp[1];

    y_map[0] = y_map_tmp[0];
    y_map[1] = y_map_tmp[1];

    render_frame = true;
  }
}

void keyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_E && action == GLFW_PRESS) {
    start_color.x = random01(gen);
    start_color.y = random01(gen);
    start_color.z = random01(gen);
    start_color.w = random01(gen);

    std::uniform_real_distribution<float> ex(start_color.x, 1);
    end_color.x = ex(gen);
    std::uniform_real_distribution<float> ey(start_color.y, 1);
    end_color.y = ey(gen);
    std::uniform_real_distribution<float> ez(start_color.z, 1);
    end_color.z = ez(gen);
    std::uniform_real_distribution<float> ew(start_color.w, 1);
    end_color.w = ew(gen);

    render_frame = true;

    return;
  }

  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    x_map[0] = orig_x_map[0];
    x_map[1] = orig_x_map[1];

    y_map[0] = orig_y_map[0];
    y_map[1] = orig_y_map[1];

    render_frame = true;

    return;
  }


  double scale_x = std::fabs(x_map[0] - x_map[1]) / 100;
  double scale_y = std::fabs(y_map[0] - y_map[1]) / 100;
  switch(key) {
    case GLFW_KEY_UP:
      y_map[0] += scale_y;
      y_map[1] += scale_y;

      render_frame = true;
      break;

    case GLFW_KEY_DOWN:
      y_map[0] -= scale_y;
      y_map[1] -= scale_y;

      render_frame = true;
      break;

    case GLFW_KEY_RIGHT:
      x_map[0] += scale_x;
      x_map[1] += scale_x;

      render_frame = true;
      break;

    case GLFW_KEY_LEFT:
      x_map[0] -= scale_x;
      x_map[1] -= scale_x;

      render_frame = true;
      break;
  }
}

void scollInput(GLFWwindow* window, double xoffset, double yoffset) {
  std::cout << xoffset << " : " << yoffset << std::endl;

  double scale_x = std::fabs(x_map[0] - x_map[1]) / 40;
  double scale_y = std::fabs(y_map[0] - y_map[1]) / 40;
  if(yoffset > 0) {
    std::cout << "> 0" << std::endl;
    x_map[0] += scale_x;
    x_map[1] -= scale_x;
    y_map[0] += scale_y;
    y_map[1] -= scale_y;
  }else if(yoffset < 0) {
    std::cout << "< 0" << std::endl;
    x_map[0] -= scale_x;
    x_map[1] += scale_x;
    y_map[0] -= scale_y;
    y_map[1] += scale_y;
  }

  render_frame = true;
}

int main(int argc, char** argv) {
  if(!glfwInit()) {
    std::cout << "glfw is not so ok" << std::endl;
    exit(1);
  }

  // set window hints
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

  // make the just created context the current context
  glfwMakeContextCurrent(window);

  // init glew
  glewExperimental = GL_TRUE;
  GLenum glew_init = glewInit();
  if(glew_init != GLEW_OK) {
    std::cout << "glew is not so ok: " << glewGetErrorString(glew_init) << std::endl;
    exit(1);
  }

  // set mouse callback to register click input
  glfwSetMouseButtonCallback(window, mouseClick);
  glfwSetCursorPosCallback(window, mouseMove);
  glfwSetKeyCallback(window, keyPressed);
  glfwSetScrollCallback(window, scollInput);

  // those two vertices fill the whole window
  // where (0, 0) is in the middle of the screen
   float fullscreen_verts[] = {
      -1.0f, 1.0f,
      -1.0f, -1.0f,
      1.0f, -1.0f,

      -1.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, -1.0f
    };

  // vbo, vao, shader and program of the actual mandelbrot
  VertexObject mandelbrot_vertex_o = VertexObject("shader.frag", fullscreen_verts, 6, 2);

  // get unifrom variables from shader
  // uniforms are global variables which are shared between GPU and CPU
  uniforms.uni_width = glGetUniformLocation(mandelbrot_vertex_o.getProgram(), "width");
  uniforms.uni_height = glGetUniformLocation(mandelbrot_vertex_o.getProgram(), "height");

  uniforms.uni_x_map = glGetUniformLocation(mandelbrot_vertex_o.getProgram(), "x_map");
  uniforms.uni_y_map = glGetUniformLocation(mandelbrot_vertex_o.getProgram(), "y_map");

  uniforms.uni_start_color = glGetUniformLocation(mandelbrot_vertex_o.getProgram(), "start_color");
  uniforms.uni_end_color = glGetUniformLocation(mandelbrot_vertex_o.getProgram(), "end_color");

  // now initialize the selection rect
  // we do this now because the opengl context has to initialized first
  selection_rect_vertex_o = new VertexObject("rect.frag", rect,  6, 2);

  // Enable blending
  // So that our selection rect is transparent
  glEnable(GL_BLEND);                             // blend the computed fragment color values with the values in the color buffers
  glBlendFunc(GL_ONE, GL_ONE);     // specifies how should be blended

  // do this while the window should not close
  while(!glfwWindowShouldClose(window)) {
    if(render_frame || (mouse_down && !get_last_mouse_pos)) {
      render_frame = false;

      // clear buffer
      glClear(GL_COLOR_BUFFER_BIT);

      // use mandelbrot program (aka shader)
      glUseProgram(mandelbrot_vertex_o.getProgram());

      // setting uniforms
      glUniform1d(uniforms.uni_width, (double)WIDTH);
      glUniform1d(uniforms.uni_height, (double)HEIGHT);

      // set the mapping range for the complex plane
      glUniform2d(uniforms.uni_x_map, x_map[0], x_map[1]);
      glUniform2d(uniforms.uni_y_map, y_map[0], y_map[1]);

      // Setting the colors
      glUniform4f(uniforms.uni_start_color, start_color.x, start_color.y, start_color.z, start_color.w);
      glUniform4f(uniforms.uni_end_color, end_color.x, end_color.y, end_color.z, end_color.w);

      // draw stuff using our vertex attribute object
      // which holds the memory layout of our meshrv
      glBindVertexArray(mandelbrot_vertex_o.getVertexAttribute());
      glDrawArrays(GL_TRIANGLES, 0, 6);

      if(mouse_down && !get_last_mouse_pos) {
        glUseProgram(selection_rect_vertex_o->getProgram());
        glBindVertexArray(selection_rect_vertex_o->getVertexAttribute());
        glDrawArrays(GL_TRIANGLES, 0, 6);
      }

      // replace frame buffer
      glfwSwapBuffers(window);
    }

    // listen to external user input eg mouse keyboard
    glfwPollEvents();

  }

  // close context
  glfwDestroyWindow(window);
  glfwTerminate();

  delete selection_rect_vertex_o;     // delete stuff in the heap

  return 0;
}
