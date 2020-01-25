#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <time.h>
#include "drawings.h"
using namespace std;

float purple[3] = {178.0/255,0,255.0/255};
float red[3] = {255/255,16.0/255,0};
float green[3] = {44.0/255,204.0/255,0};
float yellow[3] = {255/255,221.0/255,0};
float orange[3] = {255/255,106.0/255,0};



pixel::pixel(){
}
pixel::pixel(int nx, int ny, int nfruit){
  x = nx;
  y = ny;
  fruit = nfruit;
}


void fillPixel(pixel p){
  float * colorV;
  switch(p.fruit){
    case grape:
      colorV = purple;
      break;
    case apple:
      colorV = red;
      break;
    case banana:
      colorV = yellow;
      break;
    case pear:
      colorV = green;
      break;
    case orangef:
      colorV = orange;
      break;
    default:
      cout << "ERROR NO COLOR" << endl;
  }
  glColor3fv(colorV);
  glRecti(borderS+p.x*pixelW,borderS+p.y*pixelH,30+(p.x+1)*pixelW,borderS+(p.y+1)*pixelH);
}

void drawGrid(){
  //drawing grid
  glColor3f(0.8, 0.8, 0.8);

  for(int i = 30; i < 400; i+=34){
    glBegin(GL_LINES);
      glVertex2i (i, 30);
      glVertex2i (i, 770);
    glEnd();
  }
  for(int i = 30; i < 800; i+=37){
    glBegin(GL_LINES);
      glVertex2i (30, i);
      glVertex2i (370,i);
    glEnd();
  }

  //GREEN BORDER
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
    glVertex2i (20, 20);
    glVertex2i (380,20);
    glVertex2i (20, 790);
    glVertex2i (380,790);
    glVertex2i (20, 20);
    glVertex2i (20, 790);
    glVertex2i (380, 20);
    glVertex2i (380, 790);
  glEnd();
}

void drawScore(const char * s){
  glColor3f (0.0, 1.0, 0.0);
  glRasterPos2i(40, 775);
  glColor3f (0.0, 1.0, 0.0);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)s);
}
