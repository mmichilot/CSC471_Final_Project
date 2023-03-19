#ifndef COMMON_H
#define COMMON_H

#include <string>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory);
void saveImage(const char *path, int width, int height, unsigned int FBO);

#endif