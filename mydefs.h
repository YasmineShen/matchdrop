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
#define OFFSET_2 2

// 一维数组表示的棋盘结构
typedef struct board {
    char tiles[BRDSZ * BRDSZ]; // 一维数组存储棋盘
    char hawk; // 鹰格子
    int width; // 棋盘宽度
    int height; // 棋盘高度
    int parent; // 父节点索引
    int move_col; // 移动的列
} board;

typedef struct state {
    board boards[MAXBRDS]; // 棋盘数组
    int f; // 当前处理的索引
    int r; // 最后一个棋盘的索引
} state;


#include "md.h"