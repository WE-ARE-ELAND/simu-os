#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <fstream>
using namespace std;

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
    int BlockNum;     // 块号
    std::string data; // 数据
};

/*
 * 磁盘管理类
 */
class DiskManager
{
public:
    DiskBlock MyDisk[1024];                            // 磁盘，有1024个块
    vector<short> fatList;                             // 文件分配表，默认为-2空闲
    unordered_map<string, short> fileNameToNumOfBlock; // 文件名与FAT表的映射
    short freeBlocks[10][101];                         // 空闲块组10组，每组最多100个空闲块，第一位存空闲块数，第二位存上一组组号
    int FreeDataBlockNum;                              // 空闲数据区的块数
    int FreeSwapBlockNum;                              // 空闲交换区的块数

public:
    DiskManager();
    static DiskManager *getInstance();
    // 初始化操作
    void init();
    void initDisk();
    void initFat();
    void initFreeBlocks();
    // 最微操作
    short allocateBlock();
    void writeBlock(short num_block, string data);
    std::string readBlock(short block_num);
    int freeBlock(short num_block);
    // 外部函数
    int AllocateBlocks(string fileName, int size, string data);
    void DeallocateBlocks(string fileName);
    string ReadFileDataFromDisk(string fileName);
    void readSwapBlock(short blockNum, string &buffer);
    void writeSwapBlock(short blockNum, string buffer);
    // 输出函数
    void dumpFile();        // 将磁盘数据写入物理文件
    void PrintMyDisk();     // 打印磁盘数据
    void printFreeBlocks(); // 打印空闲盘块状态
};