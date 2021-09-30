#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "game.h"

#ifndef __ZYLIB__
#include "zylib.h"
#endif

#ifndef __ZYRANDOM__
#include "zyrandom.h"
#endif

#define BIRD_WIDTH 10


const DIFFICULTY easy_dif = {EASY , 1 , 3 , 45 , "\e[32m休闲模式\e[0m", DEFAULT_ROW, DEFAULT_COL};
const DIFFICULTY medium_dif = {MEDIUM , 1.1 , 5 , 40 , "\e[34m正常模式\e[0m", DEFAULT_ROW, DEFAULT_COL };
const DIFFICULTY hard_dif = {HARD, 1.25, 7, 25, "\e[33m挑战模式\e[0m", DEFAULT_ROW, DEFAULT_COL };


void Game_frame()
{
    DIFFICULTY game_difficulty = medium_dif; // 初始化难度为medium
    int score, best_score = 0;
    coord origin = {0, 2};
    char key;

    while (true)
    {
        // 重置控制台大小，清屏
        SetConsoleSize(DEFAULT_COL, DEFAULT_ROW + 6);
        ClearAll();

        // 打印菜单
        Welcome(DEFAULT_COL , game_difficulty);

        switch ((char)getch())
        {
            // 空格键，进入游戏
            case ' ':
                while(true)
                {
                    // 从文件读取最高分
                    best_score = GetBestFromFile("score.txt");

                    // 按照设置重置窗口大小，清屏
                    SetConsoleSize(game_difficulty.COL, game_difficulty.ROW + 6);
                    ClearAll();

                    // 打印游戏边框
                    MoveCursor(origin.X, origin.Y - 1);
                    PrintUI(game_difficulty.ROW, game_difficulty.COL);

                    // 进行游戏，并返回最终得分
                    // 这个函数是核心代码
                    score = Game_main(origin, game_difficulty, best_score);

                    // 清除最下面两行字，使界面更简洁
                    MoveCursor(0,origin.Y + game_difficulty.ROW + 1);
                    printf("\e[2K\n\e[2K");

                    // 比较此次得分与最高分，如果更高则写入文件
                    best_score = (score > best_score) ? PrintBestIntoFile("score.txt", score) : best_score;

                    // 打印游戏结束的提示框
                    PrintMessageBox(game_difficulty.COL/2 + origin.X - 16, game_difficulty.ROW/2 + origin.Y- 6 , score, best_score);

                    // 等待用户按键，按q退出，这里与PrintMessageBox函数强耦合，如果需要更改按键，要一起更改
                    while (true)
                    {
                        key = getch();
                        if (key == 'q') return;
                        else if (key == '\r' || key == ' ') break;
                        else continue;
                    }
                    if (key != ' ') break;

                    ClearAll(); // 清屏
                }
                break;
            
            // 退出
            case EXIT_KEY:
                return;
            
            // 显示帮助
            case HELPS_KEY:
                Game_help(DEFAULT_COL);
                break;

            // 清除记录
            case CLS_KEY:
                Game_clear("score.txt", DEFAULT_COL);
                break;

            // 显示VERSION
            case VERSION_KEY:
                Game_version(DEFAULT_COL);
                break;

            // 设置难度
            case DIF_KEY:
                game_difficulty = Game_dif(DEFAULT_COL , game_difficulty);
                break;
            default:
                break;
        }
    }
}

int Game_main(coord __origin, DIFFICULTY __game_dif, int __best_score)
{
    const int __col = __game_dif.COL,                       // 列数
              __row = __game_dif.ROW;                       // 行数
    const int barrier_width = __game_dif.BAR_WIDTH ,        // 障碍物宽度
              barrier_space = __game_dif.BAR_SPACE ,        // 障碍物间隔
              barrier_num = __col / barrier_space + 2 ;     // 障碍物的数量
    const double speed = __game_dif.SPEED;                  // 刷新速度
    const double velocity = 0.15 * speed;                   // 鸟向下的加速度（那我为什么用velocity？不改了）
    const double v_ini = speed;                             // 鸟向上的初速度

    CSTRING barrier = PrintNStr((unsigned int)barrier_width-2, "═"),        // 一串═
            barrier_cls = PrintNStr((unsigned int)barrier_width-2, " ");    // 一串空格，用来清屏

    coord barrier_pos = {__col - barrier_width + 10, 0},     // 最左边障碍物坐标
          current_pos,                                      // 每个障碍物的坐标
          bird_pos = {__col/2 - 5, __row/2},                // 鸟的坐标
          best_pos= {0,-2},                                 // 左上角的最高分数
          score_pos={bird_pos.X,-2},                        // 中间的当前分数
          dif_pos = {__col-20 , -2},                        // 右上角的难度
          help_pos = {0, __row + 1},                        // 最下面的提示语
          num_pos = {bird_pos.X + 3, bird_pos.Y - 4};       // 打印倒计时牌

    int time = 0,  top_lim, bot_lim, i, score = 0;          
    int * gap_center_array;                                 // 一个用来存储各个障碍物空隙位置的动态数组

    bool end_flag = false;                                  // game over标志

    // 动态数组赋初值
    gap_center_array = (int *)malloc( barrier_num * sizeof(int) );
    for ( i = 0; i < barrier_num; i++)
    {
        gap_center_array[i] = GenerateRandomNumber(0.25 * __row , 0.75 * __row);
    }
    
    // 打印各个文字（耦合太强了就不抽象了）
    MoveCursorRe(best_pos, __origin);
    printf("最佳成绩：%d",__best_score);
    MoveCursorRe(score_pos, __origin);
    printf("总积分：%d",score);
    MoveCursorRe(dif_pos, __origin);
    printf("当前难度：%s", __game_dif.DIF_STR);
    MoveCursorRe(help_pos, __origin);
    printf("\e[%dC按空格键使小鸟向上跳，没有操作时小鸟会下坠\n", __col/2 - 21);
    printf("\e[%dC按其他键可结束游戏\n", __col/2 - 9);

    // 打印倒计时牌，如果这时候按下了其他键则退出
    if (CountNum(num_pos , __origin) == true) return score;

    // 进入主循环
    while (true)
    {
        // 响应键盘按下，如果是空格则鸟起跳（time归零），如果是其他则退出，下同
        if (kbhit())  (getch() == ' ') ? (time = 0) : (end_flag = true);
        if (end_flag) return score;

        // 移动障碍物

        // 改变一次障碍物坐标
        barrier_pos.X--;

        // 由于障碍物有多个，所以要全部打印
        for ( i = 0; (current_pos.X = i * ( barrier_space + barrier_width) + barrier_pos.X) <= __col - barrier_width; i++)
        {
            // 赋入当前障碍物空隙的上下边界
            top_lim = gap_center_array[i] - __row * 0.18;
            bot_lim = gap_center_array[i] + __row * 0.18;

            // 打印此障碍物
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

            // 如果鸟通过了障碍物，加分
            if (current_pos.X + barrier_width == bird_pos.X)
            {
                score += 10 ;
                MoveCursorRe(score_pos, __origin);
                printf("总积分：%d",score);

                // 如果破纪录，更新最高分
                if (score > __best_score)
                {
                    __best_score = score;
                     MoveCursorRe(best_pos, __origin);
                     printf("最佳成绩：%d",__best_score);
                }

            }

            // 判断鸟碰到了障碍物，鸟死，退出游戏，返回当前积分
            if (
                    ( bird_pos.X >= (current_pos.X - BIRD_WIDTH +1) && bird_pos.X <= (current_pos.X + barrier_width -1) ) 
                    &&
                    ( bird_pos.Y <= top_lim  || bird_pos.Y >= bot_lim ) 
                ) 
            {
                MoveCursorRe(bird_pos , __origin);
                printf("--\\(×>)/--");
                return score;
            }
        }

        // 障碍物移动到边缘的处理
        if (barrier_pos.X <= 0)
        {
            barrier_pos.X = barrier_space + barrier_width ;
            for ( i = 0; i < barrier_num - 1; i++)
            {
                gap_center_array[i] = gap_center_array[i+1];
            }
            gap_center_array[barrier_num - 1] = GenerateRandomNumber(0.25 * __row, 0.75 * __row);
        }

        if (kbhit())  (getch() == ' ') ? (time = 0) : (end_flag = true);
        if (end_flag) return score;

        // 移动一次鸟

        // 清除原来鸟的字符
        MoveCursorRe(bird_pos , __origin);
        printf("           ");
        // 移动光标，打印鸟，增加计数器值
        bird_pos.Y += -v_ini + velocity * time; time++;
        MoveCursorRe(bird_pos , __origin);
        printf("--\\(˙>)/--");

        // 如果鸟碰到上下边界，鸟死，返回分数
        if ( bird_pos.Y >= __row || bird_pos.Y <= 0 )
        {
            MoveCursorRe(bird_pos , __origin);
            printf("--\\(×>)/--");
            return score;
        }

        Sleep( 40 / speed );

    }

}



// 打印游戏界面
void PrintUI(int __row, int __col)
{
    int i,j;
    system("");
    
    for ( i = 0; i < __row + 4; i++)
    {
        if (i == 0 || i == __row+1)
        {
            for ( j = 0; j < __col; j++)
            {
                printf("%s", i ? "▔" : "▁"); 
            }
        }
        printf("\n");
    }
}

// game over 提示框
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


// 菜单
void Welcome(int __col, DIFFICULTY __dif)
{
    printf("\n\n\n\e[%dC\e[31mF\e[32ml\e[33ma\e[37mp\e[35mp\e[36my \e[37mb\e[32mi\e[33mr\e[31md\e[0m\n", __col/2-6);
    
    printf("\n\n");

    #ifndef _WINDOWS_
    printf("非WIN系统，请将窗口最大化以获得最佳体验\n");
    #endif

    printf("\n");
    
    printf("\e[%dC* * * 菜   单 * * *\n\n\n", __col/2 - 10);

    printf("\e[%dC按【空格】  开始游戏\n\n", __col/2 - 11 );
    printf("\e[%dC按【 %c 】   选择难度\n\n", __col/2 - 11 , DIF_KEY);
    printf("\e[%dC按【 %c 】   清除记录\n\n", __col/2 - 11 , CLS_KEY);
    printf("\e[%dC按【 %c 】   查看帮助\n\n", __col/2 - 11 , HELPS_KEY);
    printf("\e[%dC按【 %c 】   关于...\n\n",  __col/2 - 11 , VERSION_KEY);
    printf("\e[%dC按【 %c 】   退出游戏\n\n", __col/2 - 11 , EXIT_KEY);
    printf("\n\n");
    printf("\e[%dC当前难度：%s\n", __col/2 - 10 , __dif.DIF_STR);
    if (__dif.FALG == USER)
    {
        printf("\n");
        printf("\e[%dC自定义参数：\n", __col/2 - 6 );
        printf("\e[%dC行数：%d  列数：%d  速度：%d\n" ,__col/2 - 15 , __dif.ROW, __dif.COL, (int) (__dif.SPEED * 100) );
        printf("\e[%dC障碍物宽度：%d  障碍物间距：%d", __col/2 - 15, __dif.BAR_WIDTH, __dif.BAR_SPACE );
    }
    
}

// 帮助
void Game_help(int __col)
{
    ClearAll();
    printf("\n\n\n\e[%dC\e[31mF\e[32ml\e[33ma\e[37mp\e[35mp\e[36my \e[37mb\e[32mi\e[33mr\e[31md\e[0m\n\n", __col/2-6);
    printf("\n\n");
    printf("\e[%dC* * * 帮   助 * * *\n\n", __col/2 - 10);
    printf("\n");
    printf("\e[%dC* 玩法提示 *\n\n", __col/2 - 6);
    
    printf("\e[%dC没有操作时，小鸟会快速下坠。\n", __col/2 - 14);
    printf("\e[%dC按空格键,小鸟会向上跳。\n", __col/2 - 11);
    printf("\e[%dC不要碰到上下边界及障碍物。\n", __col/2 - 13);
    printf("\e[%dC游戏中按其他键可退出游戏。\n", __col/2 - 13);
    printf("\n");
    printf("\e[%dC* 难度提示 *\n\n", __col/2 - 6);
    printf("\e[%dC障碍物宽度、速度、鸟的速度都会随难度升高而增大。\n", __col/2 - 24);
    printf("\e[%dC在“难度选择”中可以自定义难度。\n", __col/2 - 16);

    printf("\n");
    printf("\n");
    printf("\e[%dC按【任意键】返回主界面。\n\n", __col/2 - 12);
    getch();
}

// 版本
void Game_version(int __col)
{
    ClearAll();
    printf("\n\n\n\e[%dC\e[31mF\e[32ml\e[33ma\e[37mp\e[35mp\e[36my \e[37mb\e[32mi\e[33mr\e[31md\e[0m\n\n\n\n\n", __col/2-6);
    printf("\e[%dC version 1.0.0 Release\n\n\n\n", __col/2 - 11);
    printf("\e[%dCY.F.Zhao\n\n", __col/2 - 4);
    printf("\e[%dCTsinghua University\n\n\n", __col/2 - 10);
    
    printf("\e[%dC按【任意键】返回主界面。\n", __col/2 - 12);
    getch();
}

// 设置难度，允许自定义
DIFFICULTY Game_dif(int __col, DIFFICULTY __default)
{
    
    DIFFICULTY __dif = __default;
    char __key;
    ClearAll();
    printf("\n\n\n\e[%dC\e[31mF\e[32ml\e[33ma\e[37mp\e[35mp\e[36my \e[37mb\e[32mi\e[33mr\e[31md\e[0m\n\n\n\n", __col/2-6);
    printf("\e[%dC* * * 难度选择 * * *\n", __col/2 - 10);
    printf("\n\n");
    printf("\e[%dC按【  1  】 %s\n\n", __col/2 - 11 , easy_dif.DIF_STR);
    printf("\e[%dC按【  2  】 %s\n\n", __col/2 - 11 , medium_dif.DIF_STR);
    printf("\e[%dC按【  3  】 %s\n\n", __col/2 - 11 , hard_dif.DIF_STR );
    printf("\e[%dC按【  0  】 %s\n\n", __col/2 - 11 , "自定义模式" );
    printf("\e[%dC按【其他键】返回主界面\n", __col/2 - 10);
    
    switch (getch())
    {
    case '1':
       return easy_dif;
    case '2':
       return medium_dif;
    case '3':
        return hard_dif;
    case '0':
        ClearAll();
        printf("\n\n\n\e[%dC\e[31mF\e[32ml\e[33ma\e[37mp\e[35mp\e[36my \e[37mb\e[32mi\e[33mr\e[31md\e[0m\n\n\n\n", __col/2-6);
        printf("\e[%dC* * * 自定义参数 * * *\n", __col/2 - 10);
        printf("\n\n");
        printf("\e[%dC输入自定义参数，按【Enter】确认：\n", __col/2 - 16);
        printf("\e[%dC(直接按【enter】则不改变该参数)\n\n", __col/2 - 16);

        printf("\e[%dC请输入窗口宽度(60~150的整数，当前为%d):" , __col/2 - 21 , __default.COL);
        __dif.COL = InputUserSetting(50, 150, __col, __default.COL);

        printf("\e[%dC请输入窗口高度(25~35的整数，当前为%d):" , __col/2 - 21 , __default.ROW);
        __dif.ROW = InputUserSetting(25, 35, __col, __default.ROW);
        printf("\e[%dC请输入刷新速率(50~150的整数，当前为%d):" , __col/2 - 21 , (int)(__default.SPEED * 100));
        __dif.SPEED = (double)InputUserSetting(50, 150, __col, __default.SPEED * 100) / (double)100;
        printf("\e[%dC请输入障碍物宽度(2-10的整数，当前为%d):", __col/2 - 21 , __default.BAR_WIDTH);
        __dif.BAR_WIDTH = InputUserSetting(2, 10, __col, __default.BAR_WIDTH);
        printf("\e[%dC请输入障碍物之间的距离(%d-%d的整数，当前为%d):", __col/2 - 21 , __dif.BAR_WIDTH + BIRD_WIDTH*2, __dif.COL - __dif.BAR_WIDTH ,  __default.BAR_SPACE);
        __dif.BAR_SPACE = InputUserSetting(__dif.BAR_WIDTH + BIRD_WIDTH*2, __dif.COL - __dif.BAR_WIDTH , __col, __default.BAR_SPACE);

        printf("\n\e[%dC是否保存设置？\n", __col/2 - 7);
        printf("\e[%dC按【Enter】保存设置，按【q】取消并退出：\n", __col/2 - 20);

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

// 重置游戏记录
void Game_clear(const char * __restrict__ _Filename, int __col)
{
    char __key;
    FILE * __fp;
    ClearAll();
    printf("\n\n\n\e[%dC\e[31mF\e[32ml\e[33ma\e[37mp\e[35mp\e[36my \e[37mb\e[32mi\e[33mr\e[31md\e[0m\n\n\n\n", __col/2-6);
    printf("\e[%dC* * * 清除记录 * * *\n", __col/2 - 10);
    printf("\n\n");
    printf("\n\e[%dC此操作将清除保存的得分记录。\n\n", __col/2 - 14);
    printf("\n\e[%dC是否清除记录？\n\n", __col/2 - 7);
    printf("\e[%dC按【Enter】清除记录，按【q】取消并退出：\n", __col/2 - 20);
    while (true)
    {
        __key = getch();
        if (__key == '\r' )
        {
            __fp = fopen(_Filename, "w");
            fclose(__fp), __fp = NULL;
            break;
        }
        else if (__key == 'q')
        {
            break;
        }
        
    }

}
