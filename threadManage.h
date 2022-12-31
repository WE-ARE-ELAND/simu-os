#include <thread>
#include <chrono>
//  #include "memoryManage.h"
#include "DirectoryManage.h"
#include "DiskManager.h"
#include <unordered_map>

using namespace std;
// Mutex and condition variable for synchronizing access to memory
// std::mutex memory_mutex;
// std::condition_variable memory_cv;

// 本类对磁盘管理、目录管理、内存管理的类都进行了实例化，用来调用其他类的方法，其它类不可调用这个类的方法
class ThreadManager
{
public:
    DirectoryManage DIR;
    DiskManager DIM;
    // MemoryManager memoryManager;
    vector<std::thread> threads;
    mutex input_mutex;
    //"1.mkdir 2.rmdir 3.cd 4.ls 5.rm 6.touch 7.tree 8.readFile 9.writeFile 10.rename 11.exit\n"
    unordered_map<string, int> branch_table = {
        {"mkdir", 1},
        {"rmdir", 2},
        {"cd", 3},
        {"ls", 4},
        {"rm", 5},
        {"touch", 6},
        {"tree", 7},
        {"readFile", 8},
        {"writeFile", 9},
        {"rename", 10},
        {"quit", 11},
        {"default", 12},
        {"refresh", 13}};
    // 函数声明
    ThreadManager(); // 构造函数中完成初始化
    int initialize();
    void refresh();
    void showMenu();
    void data_generation_thread(string newFile);
    void delete_data_thread(string name);
    void execute_thread();
    void visualize_memory();
    void joinAllThreads();
    int generateNumber(int min, int max);
};
