#pragma once

#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "memoryManage.h"
#include "directoryManage.h"
#include "diskManager.h"

class ThreadManager
{
public:
    ThreadManager();
    void data_generation_thread(const std::string& fileName);
    void delete_data_thread(const std::string& name);
    void execute_thread(const std::string& name);
    void joinAllThreads();

    // Getters for CLI
    DirectoryManage& getDIR();
    DiskManager& getDIM();
    MemoryManager& getMem();
    std::mutex& getInputMutex() { return input_mutex_; }

    // Allow CLI to spawn background threads
    void spawnThread(std::thread&& t) { threads_.emplace_back(std::move(t)); }

private:
    void refresh();
    int generateNumber(int min, int max);
    int initialize();

    DirectoryManage DIR_;
    DiskManager DIM_;
    MemoryManager memoryManager_;
    std::vector<std::thread> threads_;
    std::mutex input_mutex_;
    std::mutex mem_mutex_;
    std::vector<std::string> opened_file_;
};