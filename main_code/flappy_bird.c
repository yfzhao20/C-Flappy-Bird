// flappy bird 
//
// version 1.0.0 release
//
// 课程大作业
// 尽量减少对windows库的依赖(但还是免不了啊)
// 重新打包封装库
// 可算是整完了
//
// by Y.F.Zhao
//
// 2021/9/1



#include <stdio.h>
#include <stdlib.h>
#include "..\library_code\zyrandom.h"
#include "..\library_code\game.h"
#include "..\library_code\ctrl.h"


int main()
{
    Randomize();                    // 初始化随机数种子
    system("");                     // 使控制字符\033(\e)可用

    HideCursor();                   // 隐藏光标
    SetTitle("Flappy Bird");        // 设置窗口标题
    LockConsoleSize();              // 锁定窗口大小（以防止输出混乱）

    Game_frame();                   // 进行游戏

    return 0;
}
