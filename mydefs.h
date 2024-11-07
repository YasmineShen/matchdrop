// Change this file in any way you like
// Do not alter driver.c / Makefile / md.h
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

#define BRDSZ 10
#define MAXBRDS 200000
#define EXTRASPACE 2


typedef struct board {
    char tiles[BRDSZ * BRDSZ]; 
    char hawk; 
    int moveCol;
    int parent;
    int width; 
    int height;  
} board;

typedef struct state {
    board boards[MAXBRDS]; 
    int r; 
    int f;
} state;


#include "md.h"
