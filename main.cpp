#include <GL/freeglut.h>
#include <GL/glut.h>

#include <complex>
#include <iostream>

#include <cmath>

// window
const unsigned int W = 512;
const unsigned int H = 512;
const unsigned int TOTAL_SIZE = W * H;

// Mandelbrot
const unsigned int MAX_ITERATIONS = 1000;
long double x_map[2] = {-2.5, 1};
long double y_map[2] = {-1, 1};

/*
f(z) = z^2 + c

z := int
c := complex    // represented by pixels

pixel_x := map to -2.5, 1       // real part of c
pixel_y := map to -1, 1         // imaginary part of c
*/

long double mapPixel(const long double& p, const long double* map, const long double& orig_Width) {
  long double f = p / orig_Width;
  long double real_part = f * abs(map[0] - map[1]);
  real_part = real_part + map[0];

  return real_part;
}

float* generateMandelBrot() {
  float* pixels = new float[TOTAL_SIZE * 3];
  unsigned int iterator = 0;
  
  for(unsigned int yi = 0; yi < H ; yi++) {
    for(unsigned int xi = 0; xi < W; xi++) {

      long double x = mapPixel((long double)xi, x_map, (long double)W);
      long double y = mapPixel((long double)yi, y_map, (long double)H);

      std::complex<long double> c;
      c.real(x);
      c.imag(y);

      std::complex<long double> last_z = 0.0;
      unsigned int iteration = 0;
      while(last_z.real() * last_z.real() + last_z.imag() * last_z.imag() <= 4.0L && iteration <= MAX_ITERATIONS) {
        last_z = last_z * last_z + c;

        iteration++;
      }

      if(iteration > MAX_ITERATIONS / 2) {
        pixels[iterator] = 255;
        pixels[iterator + 1] = 255;
        pixels[iterator + 2] = 255;
      }else{
        pixels[iterator] = 0;
        pixels[iterator + 1] = 0;
        pixels[iterator + 2] = 0;
      }

      iterator += 3;
    }
  }

  return pixels;
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, W, 0.0, H);
  glMatrixMode(GL_MODELVIEW);
}

void renderFunction() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float* pixels;

  glDrawPixels(W, H, GL_RGB, GL_FLOAT, generateMandelBrot());
  glutSwapBuffers();

  // delete pixels;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(W, H);
    glutCreateWindow("Mandelbrot");
    glutDisplayFunc(renderFunction);
    init();
    glutMainLoop();
    return 0;
}
