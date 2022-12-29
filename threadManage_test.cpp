#include <iostream>
#include "threadManage.h"
using namespace std;
int main()
{
    ThreadManager threadManager; // 该类的构造函数会完成磁盘管理、目录管理、内存管理的初始化
    threadManager.showMenu();
    return 0;
}
