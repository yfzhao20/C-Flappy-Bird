// game.h
// 游戏的各个函数

#ifndef __GAME__
#define __GAME__


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>


#define EXIT_KEY 'q'
#define VERSION_KEY 'v'
#define HELPS_KEY 'h'
#define DIF_KEY 'k'
#define CLS_KEY 'c'

#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __CTRL__
#include "ctrl.h"
#endif

#ifndef __ZYLIB__
#include "zylib.h"
#endif

#ifndef __ZYRANDOM__
#include "zyrandom.h"
#endif


// 默认游戏窗口行数、列数
#define DEFAULT_ROW 30
#define DEFAULT_COL 80

// 难度的枚举型
typedef enum {USER, EASY, MEDIUM, HARD} DIF_FLAG;

// 用来储存游戏参数
typedef struct _DIFFICULTY{
    DIF_FLAG FALG;
    double SPEED;
    int BAR_WIDTH;
    int BAR_SPACE;
    STRING DIF_STR;
    int ROW;
    int COL;
}
DIFFICULTY;

/**** 界面函数 ****/

// 打印基本界面
// 参数：行数，列数
void PrintUI(int __row, int __col);

/**** 游戏函数 ****/

// 游戏框架
void Game_frame();

// 游戏主菜单
void Welcome(int __col, DIFFICULTY __dif);

// 主函数
int Game_main(coord __origin,  DIFFICULTY __game_dif, int __best_score);

// 选择游戏难度
DIFFICULTY Game_dif(int __col, DIFFICULTY __default);

// 清除得分数据
void Game_clear(const char * __restrict__ _Filename, int __col);

// 打开游戏帮助
void Game_help(int __col);

// 关于...
void Game_version(int __col);

// 打印游戏结束消息框
void PrintMessageBox(int x, int y, int __score, int __best_score);


#endif