#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <process.h>
#include <conio.h>
#include "..\library_code\zylib.h"
#include "..\library_code\zyrandom.h"
#include "global.h"


///////////////////////////////////////////////////////////

DIFFICULTY easy_dif = {EASY , 1 , 3 , 45 , "\033[32m休闲模式\033[0m", DEFAULT_ROW, DEFAULT_COL};
DIFFICULTY medium_dif = {MEDIUM , 1.15 , 5 , 40 , "\033[34m正常模式\033[0m", DEFAULT_ROW, DEFAULT_COL };
DIFFICULTY hard_dif = {HARD, 1.25, 7, 25, "\033[33m挑战模式\033[0m", DEFAULT_ROW, DEFAULT_COL };


int Game_main(COORD __origin, DIFFICULTY __game_dif, int __best_score)
{
    const int __col = __game_dif.COL, __row = __game_dif.ROW;
    const double speed = __game_dif.SPEED;
    const int barrier_width = __game_dif.BAR_WIDTH , bird_width = 10;
    CSTRING barrier, barrier_cls;
    const double velocity = 0.15 * speed;
    const double v_ini = speed;

    const int barrier_space = __game_dif.BAR_SPACE;
    int barrier_num = __col / barrier_space + 2 ;

    COORD barrier_pos = {__col - barrier_width + 1, 0}, bird_pos = {__col/2 - 5, __row/2}, current_pos;
    COORD best_pos= {0,-2}, 
        score_pos={bird_pos.X,-2}, 
        dif_pos = {__col-20 , -2}, 
        help_pos = {0, __row + 1}, 
        num_pos = {bird_pos.X + 3, bird_pos.Y - 4};

    int time = 0,  top_lim, bot_lim, i, score = 0;
    int * gap_center_array;

    bool end_flag = false, jump_flag = false;

    barrier = PrintNStr((unsigned int)barrier_width-2, "═");
    barrier_cls = PrintNStr((unsigned int)barrier_width-2, " ");

    gap_center_array = (int *)malloc( barrier_num * sizeof(int) );
    for ( i = 0; i < barrier_num; i++)
    {
        gap_center_array[i] = GenerateRandomNumber(0.25 * __row , 0.75 * __row);
    }
    
    MoveCursorRe(best_pos, __origin);
    printf("最佳成绩：%d",__best_score);
    MoveCursorRe(score_pos, __origin);
    printf("总积分：%d",score);
    MoveCursorRe(help_pos, __origin);
    printf("\033[%dC按空格键使小鸟向上跳，没有操作时小鸟会下坠\n", __col/2 - 21);
    printf("\033[%dC按其他键可结束游戏\n", __col/2 - 9);
    MoveCursorRe(dif_pos, __origin);
    printf("当前难度：%s", __game_dif.DIF_STR);


    for ( i = 3 ; i >= 0 ; i--)
    {
        MoveCursorRe(num_pos , __origin);
        PaintNum(i);
        Sleep(1000);
        if (kbhit() && (getch() != ' ')) return score;
        MoveCursorRe(num_pos, __origin);
        PaintNum(-1);
    }

    while (true)
    {
        if (kbhit())  (getch() == ' ') ? (jump_flag = true) : (end_flag = true);
        if (end_flag) return score;

        barrier_pos.X--;

        for ( i = 0; (current_pos.X = i * ( barrier_space + barrier_width) + barrier_pos.X) <= __col - barrier_width; i++)
        {
            top_lim = gap_center_array[i] - __row * 0.18;
            bot_lim = gap_center_array[i] + __row * 0.18;
            for ( current_pos.Y = 0; current_pos.Y < __row ; current_pos.Y++)
            {   
                MoveCursorRe(current_pos, __origin);
                if ((current_pos.Y < top_lim ) || (current_pos.Y > bot_lim ) )
                    (current_pos.X == 0) ? printf(" %s  ",barrier_cls) : printf("┃%s┃ ", barrier_cls);
                else if ( current_pos.Y == top_lim)
                    (current_pos.X == 0) ? printf(" %s  ",barrier_cls) : printf("┗%s┛ ", barrier);
                else if ( current_pos.Y == bot_lim)
                    (current_pos.X == 0) ? printf(" %s  ",barrier_cls) : printf("┏%s┓ ", barrier);
            }

            if (current_pos.X + barrier_width == bird_pos.X)
            {
                score += 10 ;
                MoveCursorRe(score_pos, __origin);
                printf("总积分：%d",score);
                if (score > __best_score)
                {
                    __best_score = score;
                     MoveCursorRe(best_pos, __origin);
                     printf("最佳成绩：%d",__best_score);
                }

            }
            if (
                    ( bird_pos.X >= (current_pos.X - bird_width +1) && bird_pos.X <= (current_pos.X + barrier_width -1) ) 
                    &&
                    ( bird_pos.Y <= top_lim  || bird_pos.Y >= bot_lim ) 
                ) 
            {
                MoveCursorRe(bird_pos , __origin);
                printf("--\\(×>)/--");
                return score;
            }
        }
        if (barrier_pos.X <= 0)
        {
            barrier_pos.X = barrier_space + barrier_width ;
            for ( i = 0; i < barrier_num - 1; i++)
            {
                gap_center_array[i] = gap_center_array[i+1];
            }
            gap_center_array[barrier_num - 1] = GenerateRandomNumber(0.25 * __row, 0.75 * __row);
        }

        Sleep( 20 / speed );

        
        if (kbhit())  (getch() == ' ') ? (jump_flag = true) : (end_flag = true);
        if (end_flag) return score;


        jump_flag && (time = 0, jump_flag = false);

        // 移动一次鸟
        MoveCursorRe(bird_pos , __origin);
        printf("           ");
        bird_pos.Y += -v_ini + velocity * time;
        MoveCursorRe(bird_pos , __origin);
        printf("--\\(˙>)/--");

        if ( bird_pos.Y >= __row || bird_pos.Y <= 0 )
        {
            MoveCursorRe(bird_pos , __origin);
            printf("--\\(×>)/--");
            return score;
        }

        Sleep( 15 / speed );
        time++;

    }

}

// 此函数改编自GetIntegerFromKeyboard()
int GetInteger()
{
    int __result;
    char __ch;
    STRING __str = GetStringFromKeyboard();
    if (IsStringEqual(__str, "")) return 0;
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
        if (!(__n = GetInteger() ))  __n = __default;
        if ((__n <= __max) && (__n >= __min)) return __n;
        printf("%s\r\033[2K\033[%dC 数据不合法，请重新输入: ", __flag ? "\033[1A" : "", __col/2 - 13);
        __flag = true;
    }
}


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


// 打印游戏界面，并返回内部空白区域左上角的绝对坐标。 返回值在之后的程序中用作原点。
COORD PrintUIPos(int __row, int __col)
{
    int i,j;
    COORD __pos;
    system("");
    
    for ( i = 0; i < __row + 4; i++)
    {
        if (i == 1)
            __pos = GetPos();

        if (i == 0 || i == __row+1)
        {
            for ( j = 0; j < __col; j++)
            {
                printf("%s", i ? "▔" : "▁");    // 第一行和最后一行打印█。原来是打印空格，后来发现太慢了
            }
        }
        printf("\n");
    }
    return __pos;
}

void PrintMessageBox(int x, int y, int __score, int __best_score)
{
    MoveCursor(x,y);   printf("╔══════════════════════════════╗");
    MoveCursor(x,y+1); printf("║                              ║");
    MoveCursor(x,y+2); printf("║          Game Over!          ║");
    MoveCursor(x,y+3); printf("║                              ║");
    MoveCursor(x,y+4); printf("║        您的得分为%6d      ║", __score);
    MoveCursor(x,y+5); printf("║      当前最高得分为%6d    ║", __best_score);
    MoveCursor(x,y+6); printf("║                              ║");
    MoveCursor(x,y+7); printf("║       按【空格】再来一局     ║");
    MoveCursor(x,y+8); printf("║      按【Enter】返回主界面   ║");
    MoveCursor(x,y+9); printf("║        按【 q 】退出         ║");
    MoveCursor(x,y+10); printf("║                              ║");
    MoveCursor(x,y+11); printf("╚══════════════════════════════╝");
}

// 此函数原始版本来自https://blog.csdn.net/weixin_45631252/article/details/103244899
void MoveCursor(int __x, int __y)
{
    COORD __position;
    __position.X = __x;
    __position.Y = __y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), __position);
}

void MoveCursorRe(COORD __pos, COORD __origin)
{
    COORD __position;
    __position.X = __pos.X + __origin.X;
    __position.Y = __pos.Y + __origin.Y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), __position);
}

// 此函数原始版本来自https://zhidao.baidu.com/question/201346514.html?qbl=relate_question_1
COORD GetPos()
{
    COORD __position;
    CONSOLE_SCREEN_BUFFER_INFO Info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&Info);
    __position.Y=Info.dwCursorPosition.Y;
    __position.X=Info.dwCursorPosition.X;
    return __position;
}

void HideCursor()
{
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = sizeof(cursor);
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor);
}



void Welcome(int __col, DIFFICULTY __dif)
{
    printf("\n\n\n\033[%dC\033[31mF\033[32ml\033[33ma\033[37mp\033[35mp\033[36my \033[37mb\033[32mi\033[33mr\033[31md\033[0m\n", __col/2-6);
    
    printf("\n\n\n");
    
    printf("\033[%dC* * * 菜   单 * * *\n\n\n", __col/2 - 10);

    printf("\033[%dC按【空格】  开始游戏\n\n", __col/2 - 11 );
    printf("\033[%dC按【 k 】   选择难度\n\n", __col/2 - 11 );
    printf("\033[%dC按【 h 】   查看帮助\n\n", __col/2 - 11 );
    printf("\033[%dC按【 v 】   关于...\n\n", __col/2 - 11 );
    printf("\033[%dC按【 q 】   退出游戏\n\n", __col/2 - 11 );
    printf("\n\n");
    printf("\033[%dC当前难度：%s\n", __col/2 - 10 , __dif.DIF_STR);
    if (__dif.FALG == USER)
    {
        printf("\n");
        printf("\033[%dC自定义参数：\n", __col/2 - 6 );
        printf("\033[%dC行数：%d  列数：%d  速度：%d\n" ,__col/2 - 15 , __dif.ROW, __dif.COL, (int) (__dif.SPEED * 100) );
        printf("\033[%dC障碍物宽度：%d  障碍物间距：%d", __col/2 - 15, __dif.BAR_WIDTH, __dif.BAR_SPACE );
    }
    
}

void Game_help(int __col)
{
    system("cls");
    printf("\n\n\n\033[%dC\033[31mF\033[32ml\033[33ma\033[37mp\033[35mp\033[36my \033[37mb\033[32mi\033[33mr\033[31md\033[0m\n\n", __col/2-6);
    printf("\n\n");
    printf("\033[%dC* * * 帮   助 * * *\n\n", __col/2 - 10);
    printf("\n");
    printf("\033[%dC* 玩法提示 *\n\n", __col/2 - 6);
    
    printf("\033[%dC没有操作时，小鸟会快速下坠。\n", __col/2 - 14);
    printf("\033[%dC按空格键,小鸟会向上跳。\n", __col/2 - 11);
    printf("\033[%dC不要碰到上下边界及障碍物。\n", __col/2 - 13);
    printf("\033[%dC游戏中按其他键可退出游戏。\n", __col/2 - 13);
    printf("\n");
    printf("\033[%dC* 难度提示 *\n\n", __col/2 - 6);
    printf("\033[%dC障碍物宽度、速度、鸟的速度都会随难度升高而增大。\n", __col/2 - 24);
    printf("\033[%dC在“难度选择”中可以自定义难度。\n", __col/2 - 16);

    printf("\n");
    printf("\n");
    printf("\033[%dC按【任意键】返回主界面。\n\n", __col/2 - 12);
    getch();
}

void Game_version(int __col)
{
    system("cls");
    printf("\n\n\n\033[%dC\033[31mF\033[32ml\033[33ma\033[37mp\033[35mp\033[36my \033[37mb\033[32mi\033[33mr\033[31md\033[0m\n\n\n\n\n", __col/2-6);
    printf("\033[%dC version 0.1.2\n\n\n\n", __col/2 - 7);
    printf("\033[%dCY.F.Zhao\n\n", __col/2 - 4);
    printf("\033[%dCTsinghua University\n\n\n", __col/2 - 10);
    
    printf("\033[%dC按【任意键】返回主界面。\n", __col/2 - 12);
    getch();
}

DIFFICULTY Game_dif(int __col, DIFFICULTY __default)
{
    
    DIFFICULTY __dif = __default;
    char __key;
    system("cls");
    printf("\n\n\n\033[%dC\033[31mF\033[32ml\033[33ma\033[37mp\033[35mp\033[36my \033[37mb\033[32mi\033[33mr\033[31md\033[0m\n\n\n\n", __col/2-6);
    printf("\033[%dC* * * 难度选择 * * *\n", __col/2 - 10);
    printf("\n\n");
    printf("\033[%dC按【  1  】 %s\n\n", __col/2 - 11 , easy_dif.DIF_STR);
    printf("\033[%dC按【  2  】 %s\n\n", __col/2 - 11 , medium_dif.DIF_STR);
    printf("\033[%dC按【  3  】 %s\n\n", __col/2 - 11 , hard_dif.DIF_STR );
    printf("\033[%dC按【  0  】 %s\n\n", __col/2 - 11 , "自定义模式" );
    printf("\033[%dC按【其他键】返回主界面\n", __col/2 - 10);
    
    switch (getch())
    {
    case '1':
       return easy_dif;
    case '2':
       return medium_dif;
    case '3':
        return hard_dif;
    case '0':
        system("cls");
        printf("\n\n\n\033[%dC\033[31mF\033[32ml\033[33ma\033[37mp\033[35mp\033[36my \033[37mb\033[32mi\033[33mr\033[31md\033[0m\n\n\n\n", __col/2-6);
        printf("\033[%dC* * * 自定义参数 * * *\n", __col/2 - 10);
        printf("\n\n");
        printf("\033[%dC输入自定义参数，按【Enter】确认：\n", __col/2 - 16);
        printf("\033[%dC(直接按【enter】则不改变该参数)\n\n", __col/2 - 16);

        printf("\033[%dC请输入窗口宽度(60~150的整数，当前为%d):" , __col/2 - 21 , __default.COL);
        __dif.COL = InputUserSetting(50, 150, __col, __default.COL);

        printf("\033[%dC请输入窗口高度(25~35的整数，当前为%d):" , __col/2 - 21 , __default.ROW);
        __dif.ROW = InputUserSetting(25, 35, __col, __default.ROW);
        printf("\033[%dC请输入刷新速率(50~150的整数，当前为%d):" , __col/2 - 21 , (int)(__default.SPEED * 100));
        __dif.SPEED = (double)InputUserSetting(50, 150, __col, __default.SPEED * 100) / (double)100;
        printf("\033[%dC请输入障碍物宽度(2-10的整数，当前为%d):", __col/2 - 21 , __default.BAR_WIDTH);
        __dif.BAR_WIDTH = InputUserSetting(2, 10, __col, __default.BAR_WIDTH);
        printf("\033[%dC请输入障碍物之间的距离(%d-%d的整数，当前为%d):", __col/2 - 21 , __dif.BAR_WIDTH, __dif.COL - __dif.BAR_WIDTH ,  __default.BAR_SPACE);
        __dif.BAR_SPACE = InputUserSetting(__dif.BAR_WIDTH, __dif.COL - __dif.BAR_WIDTH , __col, __default.BAR_SPACE);

        printf("\n\033[%dC是否保存设置？\n", __col/2 - 7);
        printf("\033[%dC按【Enter】保存设置，按【q】取消并退出：\n", __col/2 - 20);

        while(true)
        {
            __key = getch();
            if (__key == '\r')
            {
                __dif.FALG = USER;
                __dif.DIF_STR = "自定义模式";
                break;
            }
            else if (__key == 'q')
                return __default;
        }
        return __dif;
    default:
        return __default;
    }
}


void PaintNum(int __num)
{
    printf("\033[33m");
    if (__num == 1)
    {
        printf(" /│  \033[1B\033[5D");
        printf("  │  \033[1B\033[5D");
        printf("  │  \033[1B\033[5D");
        printf("  │  \033[1B\033[5D");
        printf("  │  \033[1B\033[5D");
        printf("  │  \033[1B\033[5D");
        printf("  │  \033[1B\033[5D");
        printf(" ▔▔ \033[1B\033[5D");
        printf("\033[0m");
    }
    else if (__num == 2)
    {
        printf(" ▁  \033[1B\033[5D");
        printf(" /  \\\033[1B\033[5D");
        printf("    |\033[1B\033[5D");
        printf("    |\033[1B\033[5D");
        printf("   / \033[1B\033[5D");
        printf("  /  \033[1B\033[5D");
        printf(" /   \033[1B\033[5D");
        printf(" ▔▔▔\033[1B\033[5D");
        printf("\033[0m");
    }    
    
    else if (__num == 3)
    {
        printf(" ▁  \033[1B\033[5D");
        printf(" /  \\\033[1B\033[5D");
        printf("    |\033[1B\033[5D");
        printf("   / \033[1B\033[5D");
        printf("   \\ \033[1B\033[5D");
        printf("    |\033[1B\033[5D");
        printf(" \\  /\033[1B\033[5D");
        printf("  ▔ \033[1B\033[5D");
        printf("\033[0m");
    }

    else if (__num == 0)
    {
        printf(" /▔│ ╭╮\033[1B\033[6D");
        printf("▏   ││\033[1B\033[6D");
        printf("▏   ││\033[1B\033[6D");
        printf("▏▔│ ││\033[1B\033[6D");
        printf("▏ │ ││\033[1B\033[6D");
        printf("\\▁│ ╰╯ \033[1B\033[6D");
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("\033[0m");
    }
    
    
    else
    {
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("       \033[1B\033[7D");
        printf("\033[0m");
    }
    
}
