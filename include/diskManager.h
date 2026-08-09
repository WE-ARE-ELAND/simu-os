#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <fstream>

class DiskManager
{
public:
    // Constants
    static constexpr int BLOCK_SIZE = 40;
    static constexpr int NUM_BLOCKS_ = 1024;
    static constexpr int NUM_FAT_BLOCKS_ = 52;
    static constexpr int NUM_SWAP_BLOCKS_ = 124;
    static constexpr int NUM_FILE_BLOCKS_ = 900;
    static constexpr int NUM_FREE_GROUPS_ = 10;
    static constexpr int NUM_FREE_PER_GROUP_ = 100;

    DiskManager();

    // External functions
    int AllocateBlocks(std::string fileName, int size, std::string data);
    void DeallocateBlocks(std::string fileName);
    std::string ReadFileDataFromDisk(std::string fileName);
    void readSwapBlock(short blockNum, std::string &buffer);
    void writeSwapBlock(short blockNum, std::string buffer);
    std::vector<int> getAllBlocks(std::string fileName);

    // Output functions
    void dumpFile();
    void PrintMyDisk();
    void printFreeBlocks();

    // For testing (public interface for test files)
    short allocateBlock();
    int freeBlock(short num_block);
    void writeBlock(short num_block, std::string data);
    std::string readBlock(short block_num);

    int getFreeDataBlockNum() const { return FreeDataBlockNum_; }
    int getFreeSwapBlockNum() const { return FreeSwapBlockNum_; }
    const std::unordered_map<std::string, short>& getFileNameToNumOfBlock() const { return fileNameToNumOfBlock_; }
    short getFirstBlock(const std::string &fileName) const { return fileNameToNumOfBlock_.at(fileName); }

private:
    struct DiskBlock {
        int BlockNum;
        std::string data;
    };

    DiskBlock MyDisk_[NUM_BLOCKS_];
    short fatList_[NUM_BLOCKS_];
    std::unordered_map<std::string, short> fileNameToNumOfBlock_;
    short freeBlocks_[NUM_FREE_GROUPS_][NUM_FREE_PER_GROUP_ + 2];
    int FreeDataBlockNum_;
    int FreeSwapBlockNum_;

    void init();
    void initDisk();
    void initFat();
    void initFreeBlocks();
};