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
        std::cout << "[data_gen] File creation failed (name may already exist)" << std::endl;
        return;
    }
    int status = DIM_.AllocateBlocks(
        newFile.name, newFile.size / DiskManager::BLOCK_SIZE + 1, newFile.context);
    if (status == 1)
    {
        int sleep_time = generateNumber(2000, 5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        int first_block = DIM_.getFileNameToNumOfBlock().at(fileName);
        std::cout << "[data_gen] Disk blocks allocated. Blocks: ";
        for (auto i : DIM_.getAllBlocks(fileName))
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "[data_gen] Creating directory entry..." << std::endl;
        sleep_time += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        DIR_.CreateDirEntry(newFile, first_block);
        std::cout << "[data_gen] Directory entry created successfully!" << std::endl;
    }
    else
    {
        std::cout << "[data_gen] Insufficient disk space, allocation failed!" << std::endl;
    }
}

void ThreadManager::delete_data_thread(std::string name)
{
    int sleep_time = generateNumber(2000, 5000);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

    // First check if file is open, then try to delete from directory
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
        std::cout << "[data_del] File is open in memory, cannot be deleted!" << std::endl;
        return;
    }

    bool isDelFormDir = DIR_.DeleteFile(name);
    if (isDelFormDir)
    {
        std::cout << "[data_del] File removed from directory, reclaiming disk..." << std::endl;
        sleep_time += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        std::cout << "[data_del] Disk space reclaimed successfully!" << std::endl;
        std::cout << "[data_del] Reclaimed blocks: ";
        for (auto i : DIM_.getAllBlocks(name))
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        DIM_.DeallocateBlocks(name);
    }
    else
    {
        std::cout << "[data_del] File not found!" << std::endl;
    }
}

void ThreadManager::execute_thread(std::string name)
{
    DirectoryManage::File *file = DIR_.ReadFile(name);
    if (file)
    {
        std::cout << "[exec] Loading file into memory..." << std::endl;
        int sleep_time = generateNumber(2000, 5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        {
            std::lock_guard<std::mutex> lock(mem_mutex_);
            memoryManager_.allocateThreads(name, file->context);
            std::cout << std::endl;
            memoryManager_.showPage(name);
        }
        std::cout << "Open successful!" << std::endl;
        opened_file_.push_back(name);
        std::cout << DIM_.ReadFileDataFromDisk(file->name) << std::endl;
    }
    else
    {
        std::cout << "[exec] File not found or permission denied!" << std::endl;
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
    std::cout << "[" << DIR_.getUserName() << "@simuos " << path << "]$ ";
}

// Getters for CLI / tests
DirectoryManage& ThreadManager::getDIR() { return DIR_; }
DiskManager& ThreadManager::getDIM() { return DIM_; }
MemoryManager& ThreadManager::getMem() { return memoryManager_; }
