#pragma once
#include <GL/glut.h>

#define borderS 30
#define pixelW 34
#define pixelH 37
#define grape 1
#define apple 2
#define banana 3
#define pear 4
#define orangef 5


class pixel{
public:
  int x;
  int y;
  int fruit;
  pixel();
  pixel(int nx, int ny, int nfruit);
};

void fillPixel(pixel p);

void drawGrid();

void drawScore(const char * s);
