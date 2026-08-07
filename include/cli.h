#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <iostream>

#include "threadManage.h"

class CLI
{
public:
    CLI(ThreadManager& tm, DirectoryManage& dir, DiskManager& disk, MemoryManager& mem);
    void showMenu();

private:
    void refresh();

    ThreadManager& tm_;
    DirectoryManage& dir_;
    DiskManager& disk_;
    MemoryManager& mem_;
    std::mutex input_mutex_;
    std::unordered_map<std::string, int> branch_table_ = {
        {"mkdir", 1}, {"rmdir", 2}, {"cd", 3}, {"ls", 4},
        {"rm", 5}, {"touch", 6}, {"tree", 7}, {"readfile", 8},
        {"write", 9}, {"rename", 10}, {"quit", 11}, {"default", 12},
        {"refresh", 13}, {"disk", 14}, {"readDisk", 15}
    };
};
