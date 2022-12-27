#pragma once
#include <bits/stdc++.h>

const int DISK_SIZE = 40 * 1024; // 磁盘大小为 40KB
const int BLOCK_SIZE = 40;       // 每个磁盘块的大小为 40B，块内寻址需要6bit
const int NUM_BLOCKS = 1024;     // 磁盘块总数为 1024 个，块间寻址需要10bit
const int NUM_SWAP_BLOCKS = 124; // 用于存储兑换区的磁盘块数为 124 个
const int NUM_FILE_BLOCKS = 900; // 用于存储文件的磁盘块数为 900 个

struct DiskBlock
{
    int BlockNum; // 块号
    std::string data;  // 数据
};
/*----------磁盘管理类-------------*/
class DiskManager
{

public:
    DiskBlock MyDisk[1024];                  // 磁盘，有1024个块
    std::vector<short> fatList;                      // 文件分配表，默认为-2：空闲
    std::map<std::string, int> fileNameToNumOfBlock; // 文件名与FAT表的映射
    // TODO 成组连接法的数据结构
    std::stack<int> freeBlocks;
    int FreeDataBlockNum;                         // 空闲数据区的块数
    int FreeSwapBlockNum;                         // 空闲交换区的块数


public:
    DiskManager();
    static DiskManager *getInstance();

    int allocateBlock();
    void writeBlock(int num_block, char data);
    std::string read_disk_block(int block_num);
    void freeBlock(int num_block);

    int AllocateBlocks(std::string fileName, int num_blocks, std::string data);
    void DeallocateBlocks(std::string fileName);
    int SaveMmToSwap(std::string data);
    std::string ReadFileDataFromDisk(std::string fileName, int size);

    void PrintMyDisk();
};