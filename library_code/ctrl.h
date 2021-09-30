// ctrl.h
// 窗口和光标控制。
// 依赖windows系统

#ifndef __CTRL__
#define __CTRL__

#include <stdio.h>
#include <stdlib.h>

// 没试过这个跨平台是否会成功。来自https://blog.csdn.net/jingqi_se/article/details/14054191
#if defined(__WINDOWS__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN32__)
    #include <windows.h>
#endif

#ifndef __ZYLIB__
    #include "zylib.h"
#endif

typedef struct _coord {
int X;
int Y;
} coord;

/**** 光标控制（宏） ****/

// 移动光标到绝对位置
// 参数：横坐标，纵坐标
#define MoveCursor(x,y) printf("\e[%d;%dH", (y)+1, (x)+1)

// 移动光标到相对位置
// 参数：相对坐标，原点的绝对坐标。传入两个coord结构体。
#define MoveCursorRe(pos,origin) MoveCursor(pos.X+origin.X,pos.Y+origin.Y)

// 隐藏光标
#define HideCursor() printf("\e[?25l")



/**** 窗口设置 *****/

// 设置标题
#define SetTitle(string) printf("\e]0;%s\007" , string)

#define ClearAll() printf("\e[0;0H\e[2J")

// 改变控制台窗口大小，依赖Windows系统
void SetConsoleSize(int __width, int __height);

// 锁定窗口使之不能改变大小、不能最大化，依赖Windows系统
void LockConsoleSize();

#endif