#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <ctime>
#include "game.h"

using namespace std;

Game::Game(){
  srand (time(NULL));
  aShape = new shape(gBoard,rand()%6);
}
void Game::tick(int time, int & flag){
  if(time > 1){
    if(keepChecking){
      keepChecking = gBoard->matchCheck();
    }
    if(aShape->falling){
      aShape->fall(gBoard);
    }else{
      gBoard->rowCheck();
      keepChecking = gBoard->matchCheck();
      aShape = new shape(gBoard,rand()%6);
      if(aShape->collision(gBoard,'d')=='d'){
        flag = -1;
        //exit(EXIT_SUCCESS);
      }
    }
  }
}
void Game::restart(){
  gBoard = new board();
  aShape = new shape(gBoard,rand()%6);
}
int Game::getScore(){
  return gBoard->score;
}

void Game::draw(){
  gBoard->draw();
}

void Game::move(char input){
  if(input=='a'){
    aShape->moveLeft(gBoard);
  }else if(input=='d'){
    aShape->moveRight(gBoard);
  }else if(input=='w'){
    aShape->rotate(gBoard);
  }else if(input=='s'){
    aShape->fall(gBoard);
  }
}
