#ifndef SHADER_UTIL_H
#define SHADER_UTIL_H

void readShader(const char* file, char** shader, long& size_file);
GLuint loadShader(const char *fragment_path);

#endif
