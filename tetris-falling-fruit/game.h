#pragma once
#include "shapes.h"
using namespace std;

class Game {
public:
  board * gBoard = new board();
  shape * aShape;
  int test = 0;
  bool keepChecking = false;
  Game();
  void tick(int time, int & flag);
  void restart();
  int getScore();
  void draw();
  void move(char input);
};
