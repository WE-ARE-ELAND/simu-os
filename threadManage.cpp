#include "threadManage.h"
void data_generation_thread(File &newFile)
{
    // 调用磁盘分配的函数,若成功返回1，失败返回-1
    int status = AllocateBlocks(newFile->name, newFile->size, newFile->context);
    if (status == 1)
    {
        // 成功分配内存块
        int first_block = diskManager.fileNameToNumOfBlock[newFile->name]; // 获取文件名对应的第一个盘块号
    }
    else
    {
    }
}