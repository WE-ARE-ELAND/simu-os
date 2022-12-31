#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <set>
#include <queue>
#include <map>
#include <time.h>
#include <sstream>
// #include <windows.h>
#include <chrono>
using namespace std::chrono;
// system_clock::time_point time_point_now = system_clock::now();                   // 获取当前时间点
// system_clock::duration duration_since_epoch = time_point_now.time_since_epoch(); // 从1970-01-01 00:00:00到当前时间点的时长
using namespace std;
/**申请一块2560 B的内存空间模拟内存，按逻辑划分为64块，每块40B。
 * 将目录中选中的文件读入内存，并显示文件中信息（字符）。
 * 内存可以同时存放多个文件信息，每个文件固定分配8个内存块*/
const int MEMORY_SIZE = 2560;     // Total size of the memory in bytes
const int MEMORY_BLOCK_SIZE = 40; // Size of each memory block in bytes
const int NUM_MEMORY_BLOCKS = 64; // Number of memory blocks
const int DEFAULT_NUM_BLOCKS = 8; // Number of blocks assigned to each thread by default
const int INF = 2e9 + 100;
const int diskStart = 900;
const int diskEnd = 1023;
// 内存块
struct MemoryBlock
{
    int id;       // 内存块的id 独一无二
    bool is_free; // 是否被使用
    string data;    // 内存块的数据
};

// 页表项
struct PageTableEntry
{
    int memory_block_id;  // 放在哪个内存块
    int disk_id;          // 调出时候的磁盘号 默认为0     可提供900-1023
    int thread_id;        // 属于哪个进程
    bool in_memory;       // 表示页面是否在内存中（如果不在内存中，需要先调入内存）
    int last_access_time; // 最后一次被访问的时间（用于实现LRU算法）
};

// Data structure to represent a thread （PCB）
struct File
{
    int id;                                             // 进程id
    std::string name;                                   // 进程名字
    std::vector<MemoryBlock> blocks;                    // 进程包含的blocks
    std::unordered_map<int, PageTableEntry> page_table; // 页表： 页号->页表项
};
struct LruBlock
{
    int in_memory, tim, thread_id, page_id;
};

class MemoryManager
{
public:
    static int threadsID;
    static int startTime;
    std::vector<int> blocks;                   // 空闲的块
    LruBlock LRUblocks[NUM_MEMORY_BLOCKS + 5]; // 用来记录最后出现的时间和是否空闲
    std::unordered_map<int, File> threads;     // 每个进程编号对应的进程
    pair<int, int> disks[200];                 // 记录磁盘的空闲状态 first是否空闲 0 是空闲
    map <string,int> fileNameIdConvert;
    int fakeVisit[NUM_MEMORY_BLOCKS + 5];
public:
    MemoryManager();
    int findLRUblock(); // 无空闲的内存块需要置换 找最久没有使用过的
    void showCurrentBlocks(File &f);
    void allocateThreads(string threadName, string content); // size代表进程的大小 以B为单位  content代表进程的字符串
    int findDiskBlock(int thread_id, int pageid);
    // 内存回收    将这个进程从threads的map中去掉 并把这些空闲的块给腾出来
    void deleteBlock(string threadName);
    void acessPage(int thread_id, int page_num); // 访问某个进程的某个页表 需要用到页面置换算法

    int get_current_time(); // 获取当前时间 主要是为LRU算法服务
};
