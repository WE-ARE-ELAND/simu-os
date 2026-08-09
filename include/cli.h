#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>
#include <limits>

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
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> branch_table_;
    bool running_ = true;
};