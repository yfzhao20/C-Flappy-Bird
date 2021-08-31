// flappy bird 
// version 0.1.2
// 课程大作业
// 最终版。

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <process.h>
#include "..\library_code\zylib.h"
#include "..\library_code\zyrandom.h"
#include "global.h"



int main()
{
    DIFFICULTY game_difficulty = {MEDIUM , 1.15 , 5 , 40 , "\033[34m正常模式\033[0m", DEFAULT_ROW, DEFAULT_COL };
    char cmd_size[64], junk[BUFSIZ];
    int score, best_score = 0;
    COORD origin;
    char key;
    FILE * fp ;

    Randomize(); 
    system("");
    HideCursor();
    SetConsoleTitle("Flappy bird");
    // 一个奇怪的办法来改变窗口大小，来自https://zhidao.baidu.com/question/464294542.html
    sprintf(cmd_size,"mode con cols=%d lines=%d", DEFAULT_COL, DEFAULT_ROW + 6);
    system(cmd_size);

    // 锁定窗口使之不能改变大小、不能最大化，来自https://zhidao.baidu.com/question/114946611.html
    SetWindowLong(FindWindow("ConsoleWindowClass", NULL),GWL_STYLE,GetWindowLong(FindWindow("ConsoleWindowClass", NULL),GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX );

    // 从文件读取最高分


    while (true)
    {
        sprintf(cmd_size,"mode con cols=%d lines=%d", DEFAULT_COL, DEFAULT_ROW + 6);
        system(cmd_size);
        system("cls");
        Welcome(DEFAULT_COL , game_difficulty);
        switch ((char)getch())
        {
            case ' ':
                while(true)
                {
                    fp = fopen("score.txt", "a+");
                    best_score = fscanf(fp, "%[^:]: %d", junk, &best_score) == 2 ? best_score : 0;
                    fclose(fp),fp = NULL;

                    sprintf(cmd_size,"mode con cols=%d lines=%d", game_difficulty.COL, game_difficulty.ROW + 6);
                    system(cmd_size);

                    MoveCursor(0,1);
                    origin = PrintUIPos(game_difficulty.ROW, game_difficulty.COL);
                    score = Game_main(origin, game_difficulty, best_score);

                    MoveCursor(0,origin.Y + game_difficulty.ROW + 1);
                    printf("\033[2K\n\033[2K");

                    if (score > best_score)
                    {
                        best_score = score;
                        fp = fopen("score.txt", "w");
                        fprintf(fp, "Best Score: %d", score);
                        fclose(fp),fp = NULL;
                    }

                    PrintMessageBox(game_difficulty.COL/2 + origin.X - 16, game_difficulty.ROW/2 + origin.Y- 6 , score, best_score);

                    while (true)
                    {
                        key = getch();
                        if (key == 'q') return 0;
                        else if (key == '\r' || key == ' ') break;
                        else continue;
                    }
                    if (key != ' ') break;
                    system("cls");
                }
                break;
            case 'q':
                return 0;
            case 'h':
                Game_help(DEFAULT_COL);
                break;
            case 'v':
                Game_version(DEFAULT_COL);
                break;
            case 'k':
                game_difficulty = Game_dif(DEFAULT_COL , game_difficulty);
                break;
            default:
                break;
        }
    }

    return 0;
}
