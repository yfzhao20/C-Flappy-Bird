#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "global.h"

#ifndef __ZYLIB__
#include "zylib.h"
#endif


///////////////////////////////////////////////////////////

// 此函数改编自GetIntegerFromKeyboard()
int GetInteger()
{
    int __result;
    char __ch;
    STRING __str = GetStringFromKeyboard();
    if (IsStringEqual(__str, "")) return SHRT_MAX;
    if( sscanf( __str, " %d %c", &__result, &__ch) == 1)
    {
      DestroyObject( __str );
      return __result;
    }
    return -1;
}

int InputUserSetting(int __min, int __max, int __col, int __default)
{
    int __n ;
    bool __flag = false;

    while (true)
    {
        if ((__n = GetInteger() ) == SHRT_MAX)  __n = __default;
        if ((__n <= __max) && (__n >= __min)) return __n;
        printf("%s\r\e[2K\e[%dC 数据不合法，请重新输入: ", __flag ? "\e[1A" : "", __col/2 - 13);
        __flag = true;
    }
}

// 将重复的n个字符保存到字符串
STRING PrintNStr(unsigned int __time, STRING __string)
{
    STRING __output = "";
    int i;
    for ( i = 0; i < __time; i++)
    {
        __output = ConcatenateString(__output,__string);
    }
    return __output;
}

// 打印数字画？
void PaintNum(int __num)
{
    printf("\e[33m");
    if (__num == 1)
    {
        printf(" /│  \e[1B\e[5D");
        printf("  │  \e[1B\e[5D");
        printf("  │  \e[1B\e[5D");
        printf("  │  \e[1B\e[5D");
        printf("  │  \e[1B\e[5D");
        printf("  │  \e[1B\e[5D");
        printf("  │  \e[1B\e[5D");
        printf(" ▔▔ \e[1B\e[5D");
        printf("\e[0m");
    }
    else if (__num == 2)
    {
        printf(" ▁  \e[1B\e[5D");
        printf(" /  \\\e[1B\e[5D");
        printf("    |\e[1B\e[5D");
        printf("    |\e[1B\e[5D");
        printf("   / \e[1B\e[5D");
        printf("  /  \e[1B\e[5D");
        printf(" /   \e[1B\e[5D");
        printf(" ▔▔▔\e[1B\e[5D");
        printf("\e[0m");
    }    
    
    else if (__num == 3)
    {
        printf(" ▁  \e[1B\e[5D");
        printf(" /  \\\e[1B\e[5D");
        printf("    |\e[1B\e[5D");
        printf("   / \e[1B\e[5D");
        printf("   \\ \e[1B\e[5D");
        printf("    |\e[1B\e[5D");
        printf(" \\  /\e[1B\e[5D");
        printf("  ▔ \e[1B\e[5D");
        printf("\e[0m");
    }

    else if (__num == 0)
    {
        printf(" /▔│ ╭╮\e[1B\e[6D");
        printf("▏   ││\e[1B\e[6D");
        printf("▏   ││\e[1B\e[6D");
        printf("▏▔│ ││\e[1B\e[6D");
        printf("▏ │ ││\e[1B\e[6D");
        printf("\\▁│ ╰╯ \e[1B\e[6D");
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("\e[0m");
    }
    
    
    else
    {
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("       \e[1B\e[7D");
        printf("\e[0m");
    }
    
}

// 倒计时牌
bool CountNum(coord num_pos, coord __origin)
{
    int i;
    for ( i = 3 ; i >= 0 ; i--)
    {
        MoveCursorRe(num_pos , __origin);
        PaintNum(i);
        Sleep(1000);
        if (kbhit() && (getch() != ' ')) return true;
        MoveCursorRe(num_pos, __origin);
        PaintNum(-1);
    }
    return false;
}

///////////////////////////////////////////////////////////////

// 从文件中读取最好成绩
int GetBestFromFile(const char * __restrict__ _Filename)
{
    FILE * fp ;
    int best_score;
    char  junk[BUFSIZ];

    fp = fopen(_Filename, "a+");
    best_score = fscanf(fp, "%[^:]: %d", junk, &best_score) == 2 ? best_score : 0;
    fclose(fp),fp = NULL;

    return best_score;
}

// 将最好成绩写入文件
int PrintBestIntoFile(const char * __restrict__ _Filename, int __score)
{
    FILE * fp ;
    fp = fopen(_Filename, "w");
    fprintf(fp, "Best Score: %d", __score);
    fclose(fp),fp = NULL;
    return __score;
}