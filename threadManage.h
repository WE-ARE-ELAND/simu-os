#include <thread>
#include <chrono>
// #include "memoryManage.h"
#include "directoryManage.h"
// #include "DiskManager.h"

using namespace std;
// Mutex and condition variable for synchronizing access to memory
// std::mutex memory_mutex;
// std::condition_variable memory_cv;

// 本类对磁盘管理、目录管理、内存管理的类都进行了实例化，用来调用其他类的方法，其它类不可调用这个类的方法
class ThreadManager
{
public:
    DirectoryManage DIR;
    // DiskManager diskManager;
    // MemoryManager memoryManager;
    vector<std::thread> threads;
    // 函数声明
    ThreadManager(); // 构造函数中完成初始化
    int initialize();
    void showMenu();
    void data_generation_thread(DirectoryManage::File &newFile);
    void delete_data_thread(string name);
    void execute_thread();
    void visualize_memory();
    void joinAllThreads();
    int generateNumber(int min, int max);
};
