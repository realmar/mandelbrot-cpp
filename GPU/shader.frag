#version 400
out vec4 frag_colour;

uniform float width;
uniform float height;

const vec4 start_color = vec4(0.1, 0.8, 0.3, 1.0);
const vec4 end_color = vec4(1.0, 1.0, 1.0, 1.0);

void main () {
  vec2 z, c;
  c.x = gl_FragCoord.x / width * abs(-2.5 - 1.0) + -2.5;
  c.y = gl_FragCoord.y / height * abs(-1.0 - 1.0) + -1.0;

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

  if(i > 80) {
    frag_colour = vec4(0.0, 0.0, 0.0, 1.0);
  }else{
    float c = sqrt(float(i) / float(max_iterations));
    frag_colour = start_color * c + end_color * (1 - c);
  }
}
