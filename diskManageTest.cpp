#include "DiskManager.h"

int main()
{
    cout << "hello world" << endl;
    // DiskManager DIM;
    // cout << "1. 分配磁盘块 2. 回收磁盘块\n 3. 写入数据 4. 读取数据\n 5. 创建文件 6. 删除文件\n 7. 写入交换区 8. 读取交换区\n 9. 查看空闲块 10. 退出" << endl;
    // while (true)
    // {
    //     int sel;
    //     cin >> sel;
    //     switch (sel)
    //     {
    //     case 1:
    //     {
    //         short num_block = DIM.allocateBlock();
    //         cout << "分配的磁盘块为" << num_block;
    //         break;
    //     }
    //     case 2:
    //     {
    //         cout << "请输入需要回收的磁盘块号：" << endl;
    //         short num_block;
    //         cin >> num_block;
    //         if (DIM.freeBlock(num_block) == 1)
    //         {
    //             cout << num_block << "回收成功；" << endl;
    //             break;
    //         }
    //         cout << "此磁盘块已经空闲" << endl;
    //         break;
    //     }
    //     case 3:
    //     {
    //         cout << "请输入要写入的磁盘块号：" << endl;
    //         short num_block;
    //         cin >> num_block;
    //         string data;
    //         cout << "请输入要写入的文件数据：" << endl;
    //         cin >> data;
    //         DIM.writeBlock(num_block, data);
    //         break;
    //     }
    //     case 4:
    //     {
    //         cout << "请输入要读取的磁盘块号：" << endl;
    //         short num_block;
    //         cin >> num_block;
    //         string data = DIM.readBlock(num_block);
    //         cout << data;
    //         break;
    //     }
    //     case 5:
    //     {
    //         string name, data;
    //         cout << "文件名" << endl;
    //         cin >> name;
    //         int size;
    //         cout << "请输入文件大小：" << endl;
    //         cin >> size;
    //         cout << "请输入文件数据：" << endl;
    //         cin >> data;
    //         if (DIM.AllocateBlocks(name, size, data) == 1)
    //         {
    //             cout << "文件分配成功，起始磁盘号为：" << DIM.fileNameToNumOfBlock[name] << endl;
    //             break;
    //         }
    //         cout << "文件分分配失败，空间不足。" << endl;
    //         break;
    //     }
    //     case 6:
    //     {
    //         cout << "请输入要删除的文件名：" << endl;
    //         string name;
    //         cin >> name;
    //         DIM.DeallocateBlocks(name);
    //         cout << "删除成功" << endl;
    //         break;
    //     }
    //     case 7:
    //     {
    //         cout << "请输入要写入的磁盘块号：" << endl;
    //         short num_block;
    //         cin >> num_block;
    //         string data;
    //         cout << "请输入要写入的文件数据：" << endl;
    //         cin >> data;
    //         DIM.writeSwapBlock(num_block, data);
    //         break;
    //     }
    //     case 8:
    //     {
    //         cout << "请输入要读取的磁盘块号：" << endl;
    //         short num_block;
    //         cin >> num_block;
    //         string data;
    //         cout << "请输入要写入的文件数据：" << endl;
    //         cin >> data;
    //         DIM.readSwapBlock(num_block, data);
    //         cout << data;
    //         break;
    //     }
    //     case 9:
    //     {
    //         DIM.printFreeBlocks();
    //         break;
    //     }
    //     case 10:
    //     {
    //         exit(0);
    //     }
    //     default:
    //         exit(0);
    //     }
    // }
    // DIM.dumpFile();
    return 0;
}