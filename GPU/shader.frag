#version 400
out vec4 frag_colour;

void main () {
  vec2 z, c;
  c.x = gl_FragCoord.x / 512.0 * abs(-2.5 - 1.0) + -2.5;
  c.y = gl_FragCoord.y / 512.0 * abs(-1.0 - 1.0) + -1.0;

  z.x = 0.0;
  z.y = 0.0;

  int max_iterations = 100;
  int i = 0;
  for(i = 0; i < max_iterations; i++) {
    if(z.x * z.x + z.y * z.y > 4.0) break;

    float x_tmp = z.x * z.x - z.y * z.y + c.x;
    z.y = 2.0 * z.x * z.y + c.y;
    z.x = x_tmp;
  }

  frag_colour = i > 99 ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
}
