#include "directoryManage.h"
#include "diskManage.h"
#include "memoryManage.h"
#include "threadManage.h"

int main()
{
    ThreadManager threadManager; // 该类的构造函数会完成磁盘管理、目录管理、内存管理的初始化
    /* TODO:交互过程与前端显示*/

    threadManager.joinAllThreads(); // 等待其他线程执行完成
    return 0;
}
