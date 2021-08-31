#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <process.h>
#include "..\library_code\zylib.h"
#include "..\library_code\zyrandom.h"

#define DEFAULT_ROW 30
#define DEFAULT_COL 80

typedef enum {USER, EASY, MEDIUM, HARD} DIF_FLAG;

typedef struct {
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

// 打印基本界面，并给出左上角空白位置的绝对坐标；
// 参数：行数，列数
// 返回值：左上角的绝对坐标
COORD PrintUIPos(int __row, int __col);

/**** 光标函数 ****/

// 移动光标到绝对位置
// 参数：横坐标，纵坐标
void MoveCursor(int __x, int __y);

// 移动光标到相对位置
// 参数：相对坐标，原点的绝对坐标
void MoveCursorRe(COORD __pos, COORD __origin);

// 获取当前位置绝对坐标
// 返回值：当前位置绝对坐标
COORD GetPos();

// 隐藏光标
void HideCursor();

/**** 游戏函数 ****/


// 游戏提示
void Welcome(int __col, DIFFICULTY __dif);

// 主函数
int Game_main(COORD __origin,  DIFFICULTY __game_dif, int __best_score);

// 打开游戏帮助
void Game_help(int __col);

// 关于...
void Game_version(int __col);

// 选择游戏难度
DIFFICULTY Game_dif(int __col, DIFFICULTY __default);

// 打印消息框
void PrintMessageBox(int x, int y, int __score, int __best_score);

/**** 内置函数 ****/
    
// 内置函数，用以输出大型数字
void PaintNum(int __num);

// 内置函数，用以转换各种型式
int GetInteger();
STRING PrintNStr(unsigned int __time, STRING __string);