#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <time.h>
#include <queue>
using namespace std;
/**申请一块2560 B的内存空间模拟内存，按逻辑划分为64块，每块40B。
 * 将目录中选中的文件读入内存，并显示文件中信息（字符）。
 * 内存可以同时存放多个文件信息，每个文件固定分配8个内存块*/
const int MEMORY_SIZE = 2560;     // Total size of the memory in bytes
const int MEMORY_BLOCK_SIZE = 40; // Size of each memory block in bytes
const int NUM_MEMORY_BLOCKS = 64; // Number of memory blocks
const int DEFAULT_NUM_BLOCKS = 8; // Number of blocks assigned to each thread by default
const int INF = 1e9 + 100;
// 内存块
struct MemoryBlock
{
    int id;            // 内存块的id 独一无二
    bool is_free;      // 是否被使用
    string data;       // 内存块的数据
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
struct Thread
{
    int id;                                             // 进程id
    std::string name;                                   // 进程名字
    std::vector<MemoryBlock> blocks;                            // 进程包含的blocks
    std::unordered_map<int, PageTableEntry> page_table; // 页表： 页号->页表项
};
int MemoryManager::threadsID = 0;
struct LruBlock
{
    int free, tim, thread_id;
};

class MemoryManager
{
public:
    static int threadsID;
    std::vector<int> blocks;         // 空闲的块
    LruBlock LRUblocks[NUM_MEMORY_BLOCKS+5];      //用来记录最后出现的时间和是否空闲
    std::unordered_map<int, Thread> threads; // 每个进程编号对应的进程

public:
    MemoryManager()
    {
        for(int i=0;i<NUM_MEMORY_BLOCKS;i++)  //初始化空的容器
        {
            LRUblocks[i]={0, 0, -1};
            blocks.push_back(i);
        }
    }
    // struct Thread
    // {
    //     int id;                                             // 进程id
    //     std::string name;                                   // 进程名字
    //     std::vector<int> blocks;                            // 进程包含的blocks
    //     std::unordered_map<int, PageTableEntry> page_table; // 页表： 页号->页表项
    // };
    //     struct PageTableEntry
    // {
    //     int memory_block_id;  // 放在哪个内存块
    //     int disk_id;          // 调出时候的磁盘号 默认为0     可提供900-1023
    //     int thread_id;        // 属于哪个进程
    //     bool in_memory;       // 表示页面是否在内存中（如果不在内存中，需要先调入内存）
    //     int last_access_time; // 最后一次被访问的时间（用于实现LRU算法）
    // };
    void allocateThreads(string threadName, string content)  // size代表进程的大小 以B为单位  content代表进程的字符串
    {
        int needBlock = content.size();
        if(needBlock<=blocks.size())  //空闲块够分配
        {
            Thread tmp;
            tmp.id = threadsID++,tmp.name = threadName;
            int j=blocks.size()-1;
            for(int i=0;i<needBlock;i++)
            {
                MemoryBlock newBlock = {blocks[j],false,""+content[i]};  //找到空闲内存
                tmp.blocks.push_back(newBlock);                          //填入相关信息
                tmp.page_table[i]=PageTableEntry{blocks[j],0,tmp.id,true,get_current_time()};
                LRUblocks[blocks[j]] = {1, get_current_time(), tmp.id};  //记录最后一次出现的时间和是否空闲
                blocks.pop_back(),j--;                      //弹出记录的空闲内存
            }
            threads[tmp.id] = tmp;
        }
        else     //不够分配
        {

        }
    }
    int findLRUblock()  //无空闲的内存块需要置换 找最久没有使用过的
    {
        int maxn=INF,choice_id=-1;
        for (int i=0;i<NUM_MEMORY_BLOCKS;i++)
        {
            if (LRUblocks[i].tim<maxn)
            {
                
            }
        }
    }
    // 内存回收    将这个进程从threads的map中去掉 并把这些空闲的块给腾出来
    void deleteBlock(Thread &thread)
    {

    }
    

    void acessPage(int thread_id, int page_num)  //访问某个进程的某个页表 需要用到页面置换算法
    {

    }

    static int get_current_time()  //获取当前时间 主要是为LRU算法服务
    {
        return (int)time(NULL);
    }
};
