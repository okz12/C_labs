#ifndef __SUDOKUSOLVER_H_INCLUDED__
#define __SUDOKUSOLVER_H_INCLUDED__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <string.h>

using namespace std;

bool crosscheck(int board [][9],int row, int col);
bool nextsolve(int board[][9],int row,int col);
void printboard(int board[][9]);
void printfile(int board[][9]);
void readfile(int board[][9]);
void readinput(int board[][9]);

#endif