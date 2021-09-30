#include <stdio.h>
#include <stdlib.h>
#include "ctrl.h"

#ifndef __ZYLIB__
#include "zylib.h"
#endif

// 以下两个函数依赖Windows系统。


// 改变窗口大小。
// 一个奇怪的办法来改变窗口大小，来自https://zhidao.baidu.com/question/464294542.html
void SetConsoleSize(int __width, int __height)
{
    #ifdef _WINDOWS_
    char cmd_size[64];
    sprintf(cmd_size,"mode con cols=%d lines=%d", __width, __height);
    system(cmd_size);
    #endif
}

// 锁定窗口使之不能改变大小、不能最大化
// 来自https://zhidao.baidu.com/question/114946611.html
void LockConsoleSize()
{
    #ifdef _WINDOWS_
    SetWindowLong(FindWindow("ConsoleWindowClass", NULL),GWL_STYLE,GetWindowLong(FindWindow("ConsoleWindowClass", NULL),GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX );
    #endif
}