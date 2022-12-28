#pragma once
#include <bits/stdc++.h>

const int DISK_SIZE = 40 * 1024; // 磁盘大小为 40KB
const int BLOCK_SIZE = 40;       // 每个磁盘块的大小为 40B，块内寻址需要6bit
const int NUM_BLOCKS = 1024;     // 磁盘块总数为 1024 个，块间寻址需要10bit
const int NUM_SWAP_BLOCKS = 124; // 用于存储兑换区的磁盘块数为 124 个
const int NUM_FILE_BLOCKS = 900; // 用于存储文件的磁盘块数为 900 个

/*
 * 磁盘块
 */
struct DiskBlock
{
    int BlockNum; // 块号
    std::string data;  // 数据
};

/*
* 磁盘管理类
*/
class DiskManager
{
public:
    DiskBlock MyDisk[1024];                             // 磁盘，有1024个块
    std::vector<short> fatList;                         // 文件分配表，默认为-2空闲
    std::map<std::string, int> fileNameToNumOfBlock;    // 文件名与FAT表的映射
    short freeBlocks[10][101];                                // 空闲块组10组，每组最多100个空闲块，第一位存空闲块数，第二位存上一组组号
    int FreeDataBlockNum;                               // 空闲数据区的块数
    int FreeSwapBlockNum;                               // 空闲交换区的块数

public:
    DiskManager();
    static DiskManager *getInstance();
    // 最微操作
    short allocateBlock();
    void writeBlock(short num_block, char data);
    std::string read_disk_block(short block_num);
    void freeBlock(short num_block);
    // 外部函数
    short AllocateBlocks(std::string fileName, int size, std::string data);
    void DeallocateBlocks(std::string fileName);
    std::string ReadFileDataFromDisk(std::string fileName, int size);
    int SaveMmToSwap(std::string data);
    void readSwapBlock(short blockNum, char &buffer);
    void writeSwapBlock(short blockNum, char buffer);

    void PrintMyDisk();
    void dumpFile();
};