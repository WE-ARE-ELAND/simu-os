#include <cstdlib>
#include <ctime>
#include "threadManage.h"

ThreadManager::ThreadManager()
{
    srand(static_cast<unsigned>(std::time(nullptr)));
    initialize();
}

int ThreadManager::initialize()
{
    return 0;
}

void ThreadManager::data_generation_thread(std::string fileName)
{
    input_mutex_.lock();
    DirectoryManage::File newFile = DIR_.CreateFile(fileName);
    input_mutex_.unlock();
    if (newFile.name.empty())
    {
        std::cout << "数据生成进程：文件创建失败（可能已重名）" << std::endl;
        return;
    }
    std::cout << "后台处理中，您可继续执行其他命令..." << std::endl;
    int status = DIM_.AllocateBlocks(
        newFile.name, newFile.size / DiskManager::BLOCK_SIZE + 1, newFile.context);
    if (status == 1)
    {
        int sleep_time = generateNumber(2000, 5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        int first_block = DIM_.getFileNameToNumOfBlock().at(fileName);
        std::cout << "数据生成进程：文件磁盘块分配成功，分配的磁盘号为：";
        for (auto i : DIM_.getAllBlocks(fileName))
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "数据生成进程：创建目录项中..." << std::endl;
        sleep_time += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        DIR_.CreateDirEntry(newFile, first_block);
        std::cout << "数据生成进程：目录项创建成功！" << std::endl;
    }
    else
    {
        std::cout << "数据生成进程：磁盘空间不足，磁盘块分配失败！" << std::endl;
    }
}

void ThreadManager::delete_data_thread(std::string name)
{
    int sleep_time = generateNumber(2000, 5000);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

    // 先检查文件是否已打开，再尝试从目录中删除
    bool isOpened = false;
    for (const std::string &s : opened_file_)
    {
        if (s == name)
        {
            isOpened = true;
            break;
        }
    }
    if (isOpened)
    {
        std::cout << "数据删除进程：该文件已在内存中，不能被删除！" << std::endl;
        return;
    }

    bool isDelFormDir = DIR_.DeleteFile(name);
    if (isDelFormDir)
    {
        std::cout << "数据删除进程：该文件在目录中删除，正在回收外存..." << std::endl;
        sleep_time += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        std::cout << "数据删除进程：外存回收成功！" << std::endl;
        std::cout << "被回收的磁盘块号为：";
        for (auto i : DIM_.getAllBlocks(name))
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        DIM_.DeallocateBlocks(name);
    }
    else
    {
        std::cout << "数据删除进程：不存在该文件！" << std::endl;
    }
}

void ThreadManager::execute_thread(std::string name)
{
    DirectoryManage::File *file = DIR_.ReadFile(name);
    if (file)
    {
        std::cout << "执行线程：将文件调入内存中..." << std::endl;
        int sleep_time = generateNumber(2000, 5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        {
            std::lock_guard<std::mutex> lock(mem_mutex_);
            memoryManager_.allocateThreads(name, file->context);
            std::cout << std::endl;
            memoryManager_.showPage(name);
        }
        std::cout << "打开成功！" << std::endl;
        opened_file_.push_back(name);
        std::cout << DIM_.ReadFileDataFromDisk(file->name) << std::endl;
    }
    else
    {
        std::cout << "执行进程：文件不存在或权限不足！" << std::endl;
    }
}

void ThreadManager::joinAllThreads()
{
    for (auto &t : threads_)
    {
        t.join();
    }
}

int ThreadManager::generateNumber(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

void ThreadManager::refresh()
{
    std::string path = DIR_.GetPath(DIR_.getWorkDir());
    std::cout << "主线程：" << path << ">:";
}

// Getters for CLI / tests
DirectoryManage& ThreadManager::getDIR() { return DIR_; }
DiskManager& ThreadManager::getDIM() { return DIM_; }
MemoryManager& ThreadManager::getMem() { return memoryManager_; }
