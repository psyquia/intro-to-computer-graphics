#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <time.h>
#include <ctime>
#include "game.h"
using namespace std;

#define RUNNING 0
#define PAUSED 1


void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 400.0, 0.0, 800.0);
  glColor3f(1.0, 1.0, 1.0);
}

//GLOBAL VARS
Game * newGame = new Game();
int state = RUNNING;
char input = '~';
int count = 0;
int tickRate,tempRate = 400;
bool keyPressed = false, keyHeld = false;
stringstream sStream;
string scoreString;

void drawBoard(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  newGame->draw();
  if(input == 'q') exit(EXIT_SUCCESS);
  if(state==RUNNING){
    if(keyHeld){
      if(input == 's'){
        tickRate = 40;
      }
    }else{
      tickRate = tempRate;
    }
    if(keyPressed){
      if(input=='r'){
        newGame->restart();
      }else if(input=='p'){
        state = PAUSED;
      }else{
        newGame->move(input);
      }
      keyPressed = false;
    }
  }else{
    if(keyPressed){
      if(input=='p'){
        state = RUNNING;
      }
      keyPressed = false;
    }
  }
  if(state==PAUSED) scoreString = "PAUSED..."; // in displayFunc instead of timer for instant pausing
  drawGrid();
  drawScore(scoreString.c_str());
  glutSwapBuffers();
}


void timer(int tVal){
  glutTimerFunc(tickRate,timer,0);
  count++;
  if(newGame->getScore()>200){
    tempRate = 300;
  }else if(newGame->getScore()>100){
    tempRate = 350;
  }
  tickRate = tempRate;
  if(state == RUNNING){
    int f = 1;
    sStream.str(string());
    newGame->tick(count,f);
    if(f==-1)newGame->restart();
    sStream << "Score: " << newGame->getScore();
    scoreString = sStream.str();
  }
}

void getArrows(int key, int x, int y){
  if(!keyHeld) keyPressed = true;
  keyHeld = true;
  switch(key)
  {
  case GLUT_KEY_UP:
    input = 'w';
    break;
  case GLUT_KEY_DOWN:
    input = 's';
    break;
  case GLUT_KEY_LEFT:
    input = 'a';
    break;
  case GLUT_KEY_RIGHT:
    input = 'd';
    break;
  case GLUT_KEY_F1:
    input = 'r';
    break;
  default:
    input = '~';
  }
  glutPostRedisplay();
}
void getKeys(unsigned char key, int x, int y){
  if(!keyHeld) keyPressed = true;
  keyHeld = true;
  switch(key)
  {
  case 'r':
    input = 'r';
    break;
  case 'q':
    input = 'q';
    break;
  case 'p':
    input = 'p';
    break;
  default:
    input = '~';
  }
  glutPostRedisplay();
}

void upAKeys(int key, int x, int y){
  keyHeld= false;
  glutPostRedisplay();
}
void upKeys(unsigned char key, int x, int y){
  keyHeld= false;
  glutPostRedisplay();
}
int main (int argc, char** argv) {
  cout << input << "Welcome to the game!" << endl;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(50, 100);
  glutInitWindowSize(400, 800);
  glutCreateWindow("Fruitris");
  init();
  glutDisplayFunc(drawBoard);
  glutSpecialFunc(getArrows);
  glutSpecialUpFunc(upAKeys);
  glutKeyboardFunc(getKeys);
  glutKeyboardUpFunc(upKeys);
  glutIdleFunc(drawBoard);
  glutTimerFunc(0,timer,0);
  glutMainLoop();
}
