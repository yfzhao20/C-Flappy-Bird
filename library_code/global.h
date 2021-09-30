// 剩下的杂七杂八，就放在这个库里

#ifndef __GLOBAL__
#define __GLOBAL__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#ifndef _WINDOWS_
    #if defined(__WINDOWS__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN32__)
        #include <windows.h>
    #endif
#endif

#ifndef __CTRL__
    #include "ctrl.h"
#endif

#ifndef __ZYLIB__
    #include "zylib.h"
#endif

#ifndef _WINDOWS_
    #define Sleep _sleep
#endif

/**** 打印一些杂七杂八的东西 ****/
    
// 用以输出大型数字
void PaintNum(int __num);

// 用以输出倒计时牌
bool CountNum(coord __pos, coord __origin);



/**** 内置函数 ****/

// 配合InputUserSetting使用，回车返回-1
int GetInteger();

// 在一个范围中输入数字，调用了GerInteger。如果直接回车，则输出默认。
int InputUserSetting(int __min, int __max, int __col, int __default);

// 将某一个字符复制n次，写入新字符串
STRING PrintNStr(unsigned int __time, STRING __string);



/***** 文件操作 *****/

// 从文件中读取最高分
int GetBestFromFile(const char * __restrict__ _Filename);

// 向文件写入最高分
int PrintBestIntoFile(const char * __restrict__ _Filename, int __score);

#endif