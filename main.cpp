#define THREADS 16

#include <GL/freeglut.h>
#include <GL/glut.h>

// #include <complex>
#include <iostream>
#include <thread>

#include <cmath>

// window
const unsigned int W = 1024;
const unsigned int H = 1024;
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

unsigned int palette[MAX_ITERATIONS + 1][3];


long double mapPixel(const long double& p, const long double* map, const long double& orig_Width) {
  /* long double f = p / orig_Width;
  long double real_part = f * abs(map[0] - map[1]);
  real_part = real_part + map[0];

  return real_part; */

  return p / orig_Width * abs(map[0] - map[1]) + map[0];
}

void generatePalette() {
  unsigned int i = 0;

    for(unsigned int red = 0; red < 256; red += 33) {
      for(unsigned int green = 0; green < 256; green += 49) {
        for(unsigned int blue = 0; blue < 256; blue += 88) {
          palette[i][0] = red;
          palette[i][1] = green;
          palette[i][2] = blue;
          i++;
          if(i == MAX_ITERATIONS)
            return void();
      }
    }
  }
}

float* generateMandelBrot(float* pixels, unsigned int start) {
  unsigned int iterator = start * 3;
  long double x, y;

  for(unsigned int yi = 0; yi < H ; yi++) {
    for(unsigned int xi = start; xi < W; xi += THREADS) {
      if(xi + THREADS >= W)
        start = (xi + THREADS) % THREADS;

      x = mapPixel((long double)xi, x_map, (long double)W);
      y = mapPixel((long double)yi, y_map, (long double)H);

      // implementation with std::complex
      /* std::complex<long double> c(x, y);

      std::complex<long double> last_z = 0.0;
      unsigned int iteration = 0;
      while(last_z.real() * last_z.real() + last_z.imag() * last_z.imag() <= 4.0L && iteration < MAX_ITERATIONS) {
        last_z = last_z * last_z + c;

        iteration++;
      }*/


      // implementation without std::complex
      unsigned int iteration = 0;
      long double xreal = 0.0L, yimag = 0.0L;
      while(xreal * xreal + yimag * yimag < 2.0L * 2.0L && iteration < MAX_ITERATIONS) {
        /*
          fc(z) = z^2 + c
          c := (a0 + b0i)
          z := (a + bi)
          z^2 := (a + bi)^2
              = a^2 + 2abi + i * i * b * b     // i^2 = -1 (definition of i)
              => a^2 + 2abi -b^2

          => (a^2 + 2abi -b^2) + (a0 + b0i)
          // now we add those thogether and a is the new real part aka x
          // and b is the new imag part aka y
          //
          // to get the real part just add all real parts thogether
          // and to the get imaginary part add all imaginary parts thogether
          //
          // x(new real) = a^2 - b^2 + a0
          // y(new imag) = 2ab + b0
          //
          // here we omit the i because we dont know what it really is
          // and also x and y are representing the veritcal and horizontal
          // axis in a complex plane where veritcal has always a i
          // so we dont need it
          //
          // when we had i * i then we would get -1 (because i^2 = -1, because of
          // definition)
          // but this would change everything because then it would be part
          // of the real part
          // because of that 2i * 2i = -4
          // because 2 * 2 = 4
          // and i * i = -1
          // so -1 * 4 = -4
        */

        long double x_tmp = xreal * xreal - yimag * yimag + x;
        yimag = 2 * xreal * yimag + y;
        xreal = x_tmp;

        iteration++;
      }

      pixels[iterator] = (float)(palette[iteration][0]);
      pixels[iterator + 1] = (float)(palette[iteration][1]);
      pixels[iterator + 2] = (float)(palette[iteration][2]);

      /* if(iteration >= MAX_ITERATIONS) {
        pixels[iterator] = 255;
        pixels[iterator + 1] = 255;
        pixels[iterator + 2] = 255;
      }else{
        pixels[iterator] = 0;
        pixels[iterator + 1] = 0;
        pixels[iterator + 2] = 0;
      } */

      iterator += 3 * THREADS;
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

  float* pixels = new float[TOTAL_SIZE * 3];

  std::thread* threads[THREADS];
  for(unsigned int i = 0; i < THREADS; i++) {
    threads[i] = new std::thread(generateMandelBrot, pixels, i);
  }

  for(unsigned int i = 0; i < THREADS; i++) {
    threads[i]->join();
  }

  glDrawPixels(W, H, GL_RGB, GL_FLOAT, pixels);
  glutSwapBuffers();

  delete pixels;
}

int main(int argc, char** argv) {
    generatePalette();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(W, H);
    glutCreateWindow("Mandelbrot");
    glutDisplayFunc(renderFunction);
    init();
    glutMainLoop();
    return 0;
}
