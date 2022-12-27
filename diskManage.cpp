#include <bits/stdc++.h>
#include "DiskManager.h"

using namespace std;

/**
 * 构造函数
 * 初始化磁盘和FAT表
 */
DiskManager::DiskManager()
{
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        MyDisk[i].BlockNum = i;
        MyDisk[i].data = "####";
    }
    fatList.assign(1024, -2);
    FreeDataBlockNum = 900;
    FreeSwapBlockNum = 124;
}

/**
 * 创建实例
 */
DiskManager *DiskManager::getInstance()
{
    static DiskManager *instance;
    if (instance == nullptr)
    {
        instance = new DiskManager();
    }
    return instance;
}

/** 最微操作：
 * 分配一块磁盘块；
 * 向某磁盘块写入信息；
 * 读取某磁盘块信息；
 * 释放某磁盘块。
 */

/**
 * todo 利用成组连接法分配空闲块
 * @return 分配的磁盘块块号
 */
int DiskManager::allocateBlock()
{
    // todo 1. 利用成组连接法分配空闲块
    int num_block;
    if (true)
    {
        num_block = freeBlocks.top();
    }
    return num_block;
}

/**
 * 将信息写入指定磁盘块
 * @param num_block 磁盘块块号
 * @param data 需要写入的数据
 */
void DiskManager::writeBlock(int num_block, char data)
{
    MyDisk[num_block].data = data;
    if (num_block >= 900 && num_block < 1024)
    {
        FreeSwapBlockNum -= 1;
    }
    if (num_block >= 0 && num_block < 900)
    {
        FreeDataBlockNum -= 1;
    }
}

/**
 * 读取指定磁盘块的信息
 * @param num_block 磁盘块块号
 * @param data 读取的数据
 */
string DiskManager::read_disk_block(int block_num)
{
    return MyDisk[block_num].data;
}

/**
 * 释放指定磁盘块空间
 * todo 修改成组连接法的数据结构
 * @param num_block 磁盘块块号
 */
void DiskManager::freeBlock(int num_block)
{
    // 删除数据
    MyDisk[num_block].data = "####";
    // todo 修改成组连接法的数据结构

    // 修改空闲块数
    if (num_block >= 900 && num_block < 1024)
    {
        FreeSwapBlockNum += 1;
    }
    if (num_block >= 0 && num_block < 900)
    {
        FreeDataBlockNum += 1;
    }
}

/***************************************************************/

/**
 * 存储文件，为文件分配磁盘块
 * @param num_blocks 所需的磁盘块数量
 * @return 空间是否充足
 */
int DiskManager::AllocateBlocks(string fileName, int size, string data)
{
    // 1. 空闲磁盘块不足以存储相应大小的文件
    if (size > FreeSwapBlockNum)
    {
        return -1;
    }
    // 2. 分配相应大小的磁盘块
    int num_block, tmp_num_block, index = 0; // 动态分配的块号, 上一个分配的磁盘块 索引号；
    while (size--)
    {
        // 2.1 分配磁盘块
        num_block = allocateBlock();
        writeBlock(num_block, data[index]);
        // MyDisk[num_block] = data[index];
        // 2.2 将分配的盘块号写入FAT表
        {
            if (index == 0)
            {
                DiskManager::fileNameToNumOfBlock[fileName] = num_block;
            }
            else
            {
                fatList[tmp_num_block] = num_block;
            }
            tmp_num_block = num_block;
        }
        index++;
    }
    fatList[num_block] = -1;
    return 1;
}

/**
 * 删除文件，回收空闲区
 */
void DiskManager::DeallocateBlocks(string fileName)
{
    int start_num_block = DiskManager::fileNameToNumOfBlock[fileName];
    // 1. 更新FAT表
    int num_block = start_num_block;
    while (fatList[num_block] != -1)
    {
        int next_num_block = fatList[num_block];
        fatList[num_block] = -2;
        {
            // 2. 更新成组连接的数据结构
            freeBlock(num_block);
        }
        num_block = next_num_block;
    }
    fatList[num_block] = -2;
    freeBlock(num_block);
    fileNameToNumOfBlock.erase(fileName);
}

/*----------保存内存数据到交换区，返回块号-------------*/
int DiskManager::SaveMmToSwap(string data) {
    // filter
    int i = 900;
    for (; i < 1024; ++i)
    {
        if (fatList[i] == -2) {
            writeBlock(i, data[i]);
            break;
        }
    }
    return i;
}

/**
 * 读取文件数据
*/
string DiskManager::ReadFileDataFromDisk(string fileName, int size) {
    int start_num_block = fileNameToNumOfBlock[fileName];
    string data = "";
    int num_block = start_num_block;
    while (fatList[num_block] != -1)
    {
        data.append(read_disk_block(num_block));
        num_block = fatList[num_block];
    }
    data.append(read_disk_block(num_block));
    return data;
}

/**
 * 打印磁盘数据
*/
void DiskManager::PrintMyDisk()
{

    for (int i = 1; i <= 1024; i++)
    {
        cout << MyDisk[i - 1].data << " ";
        if (i % 16 == 0)
            cout << endl;
    }
};


/**
 * 测试代码
*/
int main() {
    DiskManager::getInstance()->PrintMyDisk();
    string data = "1234xxzz5678ccvv9999oooo0x0xcccc1234xxzz5678ccvv9999oooo0x0xcccc";
    DiskManager::getInstance()->AllocateBlocks("hello", 16, data);
    DiskManager::getInstance()->PrintMyDisk();
    return 0;
}