#include "threadManage.h"
void data_generation_thread(File &newFile)
{
    // 调用磁盘分配的函数,若成功返回1，失败返回-1
    int status = AllocateBlocks(newFile->name, newFile->size, newFile->context);
    if (status == 1)
    {
        // 成功分配内存块
        }
    else
    {
    }
}