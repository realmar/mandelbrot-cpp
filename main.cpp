#include <GL/freeglut.h>
#include <GL/glut.h>

#include <complex>
#include <iostream>

#include <cmath>

// window
const unsigned int W = 100;
const unsigned int H = 100;
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

long double mapPixel(const unsigned int& pixel, const long double* range, const unsigned int& old_max) {
  return abs((((old_max / 100) * pixel) / 100) * (range[0] - range[1])) + range[0];

  // return (((pixel - 0) * (range[1] - range[0])) / (old_max - 0)) + 0;
}

float* generateMandelBrot() {
  float* pixels = new float[TOTAL_SIZE * 3];

  for(unsigned int i = 0; i < TOTAL_SIZE * 3; i += 3) {
    long double x = mapPixel(i, x_map, W);
    long double y = mapPixel(i, y_map, H);

    std::cout << "x mapping: " << x << std::endl;
    std::cout << "y mapping: " << y << std::endl;

    std::complex<long double> c;
    c.real(x);
    c.imag(y);

    std::complex<long double> last_z = 0.0;
    unsigned int iteration = 0;
    while(last_z.real() <= 2.0L && last_z.imag() <= 2.0L && iteration <= MAX_ITERATIONS) {
      last_z = last_z * last_z + c;

      //std::cout << last_z << ";";
      iteration++;
    }

    if(iteration != 1)
      std::cout << iteration << ";";

    if(iteration > MAX_ITERATIONS / 2) {
      pixels[i] = 255;
      pixels[i + 1] = 255;
      pixels[i + 2] = 255;
    }else{
      pixels[i] = 0;
      pixels[i + 1] = 0;
      pixels[i + 2] = 0;
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
