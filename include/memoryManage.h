#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <memory>

class MemoryManager
{
    static constexpr int MEMORY_SIZE_ = 2560;
    static constexpr int MEMORY_BLOCK_SIZE_ = 40;
    static constexpr int NUM_MEMORY_BLOCKS_ = 64;
    static constexpr int DEFAULT_NUM_BLOCKS_ = 8;
    static constexpr int INF_ = 2000000100;
    static constexpr int DISK_START_ = 900;
    static constexpr int DISK_END_ = 1023;

    struct MemoryBlock {
        int id;
        bool is_free;
        std::string data;
    };

    struct PageTableEntry {
        int memory_block_id;
        int disk_id;
        int thread_id;
        bool in_memory;
        int last_access_time;
    };

    struct File {
        int id;
        std::string name;
        std::vector<MemoryBlock> blocks;
        std::unordered_map<int, PageTableEntry> page_table;
    };

    struct LruBlock {
        int in_memory;
        int tim;
        int thread_id;
        int page_id;
    };

public:
    MemoryManager();

    void allocateThreads(std::string threadName, std::string content);
    void deleteBlock(std::string threadName);
    void showPage(std::string threadName);
    void showCurrentBlocks(const File &f);

    std::vector<int> getFreeBlocks() const { return blocks_; }
    int getThreadCount() const { return static_cast<int>(threads_.size()); }
    int getIdByName(const std::string &name) const { return fileNameIdConvert_.at(name); }

private:
    int findLRUblock();
    int findDiskBlock(int thread_id, int pageid);
    int get_current_time();

    static int threadsID_;
    static int startTime_;

    std::vector<int> blocks_;
    LruBlock LRUblocks_[NUM_MEMORY_BLOCKS_];
    std::unordered_map<int, File> threads_;
    std::pair<int, int> disks_[DISK_END_ - DISK_START_ + 1];
    std::map<std::string, int> fileNameIdConvert_;
};
