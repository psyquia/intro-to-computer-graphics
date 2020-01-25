#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <time.h>
#include "shapes.h"
using namespace std;


coor::coor(){}
coor::coor(int nx, int ny){
  x = nx;
  y = ny;
}

board::board(){}
void board::printBi(){
  for(int i = 19; i >= 0; i--){
    for(int j = 0; j < 10; j++){
      cout << binaryBoard[j][i];
    }
    cout << endl;
  }
}
void board::printPi(){
  for(int i = 19; i >= 0; i--){
    for(int j = 0; j < 10; j++){
      cout << pixelBoard[j][i].fruit;
    }
    cout << endl;
  }
}
//

void board::draw(){
  for(int i = 0; i<10;i++){
    for(int k = 0; k<20; k++){
      if(binaryBoard[i][k] == 1){
        fillPixel(pixelBoard[i][k]);
      }
    }
  }
}

void board::removePixel(pixel p){
  binaryBoard[p.x][p.y] = 0;
  if(binaryBoard[p.x][p.y+1] == 0){
    return;
  }
  binaryBoard[p.x][p.y] = 1;
  pixelBoard[p.x][p.y].fruit = pixelBoard[p.x][p.y+1].fruit;
  removePixel(pixelBoard[p.x][p.y+1]);
}

bool board::matchCheck(){
  bool found = false;
  int mCount;
  int tempBiBoard[10][22]; // used to reduce unseen chain reactions

  for(int i = 0; i<10; i++){
    for(int k = 0; k<20; k++){
      if(binaryBoard[i][k]==1){
        tempBiBoard[i][k] = 1;
      }else{
        tempBiBoard[i][k] = 0;
      }
    }
  }

  //vertical matching
  for(int i = 0; i<10; i++){
    mCount = 0;
    for(int j = 1; j<20; j++){
      if((tempBiBoard[i][j] == 1 && tempBiBoard[i][j-1] == 1) && (pixelBoard[i][j].fruit == pixelBoard[i][j-1].fruit)){
        mCount++;
      }else{
        mCount = 0;
      }
      if(mCount == 2){
        found = true;
        score+=3;
        mCount = 0;
        removePixel(pixelBoard[i][j]);
        removePixel(pixelBoard[i][j-1]);
        removePixel(pixelBoard[i][j-2]);
        tempBiBoard[i][j] = 0;
        tempBiBoard[i][j-1] = 0;
        tempBiBoard[i][j-2] = 0;
      }
    }
  }

  //horizontal matching
  for(int i = 0; i<20; i++){
    mCount = 0;
    for(int j = 1; j<10; j++){
      if((tempBiBoard[j][i] == 1 && tempBiBoard[j-1][i] == 1) && (pixelBoard[j][i].fruit == pixelBoard[j-1][i].fruit)){
        mCount++;
      }else{
        mCount = 0;
      }
      if(mCount == 2){
        found = true;
        score+=3;
        mCount = 0;
        removePixel(pixelBoard[j][i]);
        removePixel(pixelBoard[j-1][i]);
        removePixel(pixelBoard[j-2][i]);
        tempBiBoard[j][i] = 0;
        tempBiBoard[j-1][i] = 0;
        tempBiBoard[j-2][i] = 0;
      }
    }
  }
  //diagonal matching
  for(int i = 2; i<20; i++){
    for(int j = 2; j<10; j++){
      mCount = 0;
      for(int k = -2; k<0; k++){ // forward "/" diagonals
        if((tempBiBoard[j][i] == 1 && tempBiBoard[j+k][i+k] == 1) && (pixelBoard[j][i].fruit == pixelBoard[j+k][i+k].fruit)){
          mCount++;
        }else{
          mCount = 0;
        }
        if(mCount == 2){
          found = true;
          score+=3;
          mCount = 0;
          removePixel(pixelBoard[j+k+1][i+k+1]);
          removePixel(pixelBoard[j+k][i+k]);
          removePixel(pixelBoard[j+k-1][i+k-1]);
          tempBiBoard[j+k+1][i+k+1] = 0;
          tempBiBoard[j+k][i+k] = 0;
          tempBiBoard[j+k-1][i+k-1] = 0;
        }
      }
    }
  }
  for(int i = 2; i<20; i++){
    for(int j = 0; j<8; j++){
      mCount = 0;
      for(int k = -2; k<0; k++){ // back "\" diagonals
        if((tempBiBoard[j][i] == 1 && tempBiBoard[j-k][i+k] == 1) && (pixelBoard[j][i].fruit == pixelBoard[j-k][i+k].fruit)){
          mCount++;
        }else{
          mCount = 0;
        }
        if(mCount == 2){
          found = true;
          score+=3;
          mCount = 0;
          removePixel(pixelBoard[j][i]);
          removePixel(pixelBoard[j-k][i+k]);
          removePixel(pixelBoard[j-(k-1)][i+(k-1)]);
          tempBiBoard[j][i] = 0;
          tempBiBoard[j-k][i+k] = 0;
          tempBiBoard[j-(k-1)][i+(k-1)] = 0;
        }
      }
    }
  }
  return found;
}

void board::clearRow(int rowN){
  for(int k = 0; k<10; k++){
    binaryBoard[k][rowN] = 0;
  }
  for(int i = rowN+1; i<20; i++){
    for(int k = 0; k<10; k++){
      if(binaryBoard[k][i] == 1){
        binaryBoard[k][i] = 0;
        binaryBoard[k][i-1] = 1;
        pixelBoard[k][i-1].fruit = pixelBoard[k][i].fruit;
      }
    }
  }
}
void board::rowCheck(){
  for(int i = 0; i<20; i++){
    bool del = true;
    while(del){
      int rowCount = 0;
      for(int k = 0; k<10; k++){
        if(binaryBoard[k][i] == 1) rowCount++;
      }
      if(rowCount==10){
        score+=10;
        clearRow(i);
      }else{
        del = false;
      }
    }
  }
}

shape::shape(board * boar, int mType){
  type = mType;
  if(type == I){
    rotSize = 2;
    type = I;
  }else if(type == L){
    rotSize = 4;
    type = L;
  }else if(type == LM){
    rotSize = 4;
    type = LM;
  }else if(type == S){
    rotSize = 4;
    type = S;
  }else if(type == SM){
    rotSize = 4;
    type = SM;
  }else if(type == T){
    rotSize = 4;
    type = T;
  }
  for(int i = 0; i<4; i++){
    pixels[i] = rand() % 5 + 1;
  }
  int coorI = 0;
  for(int i = 0; i<4; i++){
    for(int j = 0; j<4; j++){
      if(rotations[type][currRot][i][j] != 0){
        boar->binaryBoard[x+j][y-i] = 1;
        boar->pixelBoard[x+j][y-i] = pixel(x+j,y-i,pixels[rotations[type][currRot][i][j]-1]);
        if(coorI < 4){
          pixelCoor[coorI] = coor(x+j,y-i);
          coorI++;
        }
      }
    }
  }
}

void shape::printCoor(){
  for(int i = 0; i<4; i++){
    cout << '(' << pixelCoor[i].x << ", " << pixelCoor[i].y << ')' << endl;
  }
}
void shape::clear(board * boar){
  for(int i = 0; i<4; i++){
    boar->binaryBoard[pixelCoor[i].x][pixelCoor[i].y] = 0;
  }
}
void shape::updateB(board * boar){
  int coorI = 0;
  for(int i = 0; i<4; i++){
    for(int j = 0; j<4; j++){
      if(rotations[type][currRot][i][j] != 0){
        boar->binaryBoard[x+j][y-i] = 1;
        boar->pixelBoard[x+j][y-i] = pixel(x+j,y-i,pixels[rotations[type][currRot][i][j]-1]);
        if(coorI < 4){
          pixelCoor[coorI] = coor(x+j,y-i);
          coorI++;
        }
      }
    }
  }
}
char shape::collision(board * boar, char dir){
  int c;
  int pivotX = x+2;
  int pivotY = y-1;
  int bound;
  char ret;
  switch(dir){
    case 'l':
      ret = 'l';
      c = -1;
      bound = pivotX;
      break;
    case 'r':
      ret = 'r';
      c = 1;
      bound = pivotX;
      break;
    case 'd':
      ret = 'd';
      c = -1;
      bound = pivotY;
      break;
    case 't': //rotation collision
      ret = 't';
  }
  //calculating x/y bounds
  for(int i = 0; i<4; i++){
    for(int j = 0; j<4; j++){
      if(rotations[type][currRot][i][j] != 0){
        if(dir=='l'){
          bound = (x+j<bound) ? x+j : bound;
        }else if (dir=='r'){
          bound = (x+j>bound) ? x+j : bound;
        }else if (dir=='d'){
          bound = (y-i<bound) ? y-i : bound;
        }
      }
    }
  }
  if(dir == 'l' || dir == 'r'){
    if(bound + c < 0 || bound + c >= 10) return ret;
    for(int i = 0; i<4; i++){
      bool match = false;
      coor tmpCoor = (dir=='r') ? coor(pixelCoor[i].x+1,pixelCoor[i].y) : coor(pixelCoor[i].x-1,pixelCoor[i].y);
      if(boar->binaryBoard[tmpCoor.x][tmpCoor.y] == 1){
        for(int j = 0; j<4; j++){
          if(tmpCoor.x == pixelCoor[j].x && tmpCoor.y == pixelCoor[j].y){
            match = true;
          }
        }
        if(!match) return ret;
      }
    }
  }else if(dir == 'd'){
    if(bound + c < 0){
      return ret;
    }
    for(int i = 0; i<4; i++){
      bool match = false;
      coor tmpCoor = coor(pixelCoor[i].x,pixelCoor[i].y-1);
      if(boar->binaryBoard[tmpCoor.x][tmpCoor.y] == 1){
        for(int j = 0; j<4; j++){
          if(tmpCoor.x == pixelCoor[j].x && tmpCoor.y == pixelCoor[j].y){
            match = true;
          }
        }
        if(!match) return ret;
      }
    }
  }
  else if(dir=='t'){
    //simulating next rotation
    int coorI = 0;
    coor tmpCts[4];
    for(int i = 0; i<4; i++){
      for(int j = 0; j<4; j++){
        if(rotations[type][(currRot+1)%rotSize][i][j] != 0){
          if(coorI < 4){
            if(x+j < 0 || x+j >= 10 || y-i > 19 || y-i < 0){
              return ret;
            }
            tmpCts[coorI] = coor(x+j,y-i);
            coorI++;
          }
        }
      }
    }
    for(int i = 0; i<4; i++){
      bool match = false;
      if(boar->binaryBoard[tmpCts[i].x][tmpCts[i].y] == 1){
        for(int j = 0; j<4; j++){
          if(tmpCts[i].x == pixelCoor[j].x && tmpCts[i].y == pixelCoor[j].y){
            match = true;
          }
        }
        if(!match) return ret;
      }
    }
  }
  return 'n';
}
void shape::rotate(board * boar){
  if(falling){
    if(collision(boar,'t') != 't'){
      clear(boar);
      currRot = (currRot + 1) % rotSize;
      updateB(boar);
    }
  }
}
void shape::fall(board * boar){
  if(collision(boar,'d') != 'd'){
    clear(boar);
    y--;
    updateB(boar);
  }else{
    falling = false;
  }
}
void shape::moveLeft(board * boar){
  if(falling){
    if(collision(boar,'l') != 'l'){
      clear(boar);
      x--;
      updateB(boar);
    }
  }
}
void shape::moveRight(board * boar){
  if(falling){
    if(collision(boar,'r') != 'r'){
      clear(boar);
      x++;
      updateB(boar);
    }
  }
}
