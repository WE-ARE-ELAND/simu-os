#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <queue>
#include <map>
#include <iostream>
#include<time.h> 
#include <sstream>
#include <windows.h>
#include <iostream>
#include <chrono>
using namespace std::chrono;
system_clock::time_point time_point_now = system_clock::now(); // 获取当前时间点
system_clock::duration duration_since_epoch = time_point_now.time_since_epoch(); // 从1970-01-01 00:00:00到当前时间点的时长
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
    int id;            // 内存块的id 独一无二
    bool is_free;      // 是否被使用
    char data;       // 内存块的数据
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
    std::vector<int> blocks;         // 空闲的块
    LruBlock LRUblocks[NUM_MEMORY_BLOCKS+5];      //用来记录最后出现的时间和是否空闲
    std::unordered_map<int, File> threads; // 每个进程编号对应的进程
    pair<int,int> disks[200];                   //记录磁盘的空闲状态 first是否空闲 0 是空闲
public:
    MemoryManager()
    {
        for(int i=0;i<NUM_MEMORY_BLOCKS;i++)  //初始化空的容器
        {
            LRUblocks[i]={0, 0, -1, -1};
            blocks.push_back(i);
        }
        for(int i=0;i<200;i++)
            disks[i]={0,0};
    }

    int findLRUblock()  //无空闲的内存块需要置换 找最久没有使用过的
    {
        int maxn=INF,choice_id=-1;
        for (int i=0;i<NUM_MEMORY_BLOCKS;i++)
        {
            // cout<<i<<' '<<LRUblocks[i].tim<<endl;
            if (LRUblocks[i].tim<maxn)
                maxn = LRUblocks[i].tim, choice_id=i;
        }
        cout<<maxn<<endl;
        return choice_id;
    }

    void showCurrentBlocks(File& f)
    {
        map<int,string> mp;
        for (auto& block:f.blocks)
        {
            // cout<<block.id<<' '<<block.data<<endl;
            mp[block.id] = block.data;
        }
        for(int i=0;i<NUM_MEMORY_BLOCKS;i++)
        {
            if(i%8 == 0)
                printf("\n");
            if(mp.count(i))
                cout<<mp[i];
            else 
                printf("#");
        }
    }
    void allocateThreads(string threadName, string content)  // size代表进程的大小 以B为单位  content代表进程的字符串
    {
        int needBlock = content.size();
        File tmp;
        tmp.id = threadsID++,tmp.name = threadName;
        if(needBlock<=blocks.size())  //空闲块够分配
        {
            printf("\nCurrent availavle memory blocks is enough to allocate\n");
            int j=blocks.size()-1;
            for(int i=0;i<needBlock;i++)
            {
                MemoryBlock newBlock = {blocks[j],false,content[i]};  //找到空闲内存
                tmp.blocks.push_back(newBlock);                          //填入相关信息
                tmp.page_table[i]=PageTableEntry{blocks[j],0,tmp.id,true,get_current_time()};
                LRUblocks[blocks[j]] = {1, get_current_time(), tmp.id, i};  //记录最后一次出现的时间和是否空闲
                blocks.pop_back(),j--;                      //弹出记录的空闲内存
            }
            threads[tmp.id] = tmp;
            printf("We allovate memory blocks as follows:\n");
            for (auto& block : tmp.blocks)
                printf("%d    ", block.id);
            printf("\n");
            showCurrentBlocks(tmp);
        }
        else     //不够分配
        /* 
        1 把当前空闲块分配完    2 根据lru算法寻找出现时间最早的    3 将内存块调出内存 放入磁盘
         */
        {
            printf("\nWe don't have enough blocks and will use LRU algrothirm to find available blocks\n");
            File tmp;
            tmp.id = threadsID++,tmp.name = threadName;
            int j=blocks.size()-1, nowsize = blocks.size();
            for(int i=0;i<nowsize;i++)
            {
                MemoryBlock newBlock = {blocks[j],false,content[i]};  //找到空闲内存
                tmp.blocks.push_back(newBlock);                          //填入相关信息
                tmp.page_table[i]=PageTableEntry{blocks[j],0,tmp.id,true,get_current_time()};
                LRUblocks[blocks[j]] = {1, get_current_time(), tmp.id, i};  //记录最后一次出现的时间和是否空闲
                blocks.pop_back(),j--;                      //弹出记录的空闲内存
            }
            printf("We should throw these blocks out:\n");
            for (int i=nowsize;i<needBlock;i++)
            {
                int findBlock = findLRUblock();     //需要把这个块置换出去
                LruBlock lrublock = LRUblocks[findBlock];
                printf("%d     ",findBlock);

                threads[lrublock.thread_id].page_table[lrublock.page_id].in_memory = 0;
                threads[lrublock.thread_id].page_table[lrublock.page_id].disk_id = findDiskBlock(threads[lrublock.thread_id].page_table[lrublock.page_id].thread_id, threads[lrublock.thread_id].page_table[lrublock.page_id].memory_block_id);
                LRUblocks[findBlock].tim = get_current_time();

                int avaBlock = threads[lrublock.thread_id].page_table[lrublock.page_id].memory_block_id;
                MemoryBlock newBlock = {avaBlock,false,content[i]};
                tmp.blocks.push_back(newBlock);
                tmp.page_table[i]=PageTableEntry{avaBlock,0,tmp.id,true,get_current_time()};
            }
            printf("\nFinally the allovate memory blocks is as follows:\n");
            for (auto& block : tmp.blocks)
                printf("%d    ", block.id);
            printf("\n");
            threads[tmp.id] = tmp;
            showCurrentBlocks(tmp);
        }
    }
    int findDiskBlock(int thread_id, int pageid)
    {
        for (int i=0;i<=diskEnd-diskStart;i++)
        {
            if (disks[i].first == 0)
            {
                disks[i] = {thread_id, pageid};
                return i;
            }
        }
        return 0;
    }

    // 内存回收    将这个进程从threads的map中去掉 并把这些空闲的块给腾出来
    void deleteBlock(int threadID)
    {
        File thread = threads[threadID];
        threads.erase(threadID);
        for (auto&page :thread.page_table)
        {
            if(page.second.in_memory)         //在内存中就释放
            {
                blocks.push_back(page.second.memory_block_id);
                disks[page.second.disk_id].first = 0;
            }
        }
    }
    void acessPage(int thread_id, int page_num)  //访问某个进程的某个页表 需要用到页面置换算法
    {
        File thread = threads[thread_id];
        if(thread.page_table[page_num].in_memory)
        {
            LRUblocks[thread.page_table[page_num].memory_block_id].tim = get_current_time();
        }
        else 
        {
            //1 还有空闲的内存
            if(blocks.size() > 0)
            {
                int j=blocks.size()-1, blockId = blocks[j];
                j--, blocks.pop_back();
                for (int i=0;i<=diskEnd-diskStart;i++)
                {
                    if (disks[i].first == thread_id && disks[i].second == page_num)
                    {
                        disks[i].first = 0;
                        break;
                    }
                }
                LRUblocks[blockId] = {true,get_current_time(),thread_id,page_num};
                PageTableEntry& now_page = threads[thread_id].page_table[page_num];
                now_page = {blockId, 0, thread_id, true, get_current_time()};
            }
            else //无空闲内存 需要用到页面置换算法
            {
                int block_id = findLRUblock();    //置换的内存块
                LruBlock& lrublock = LRUblocks[block_id];
                threads[lrublock.thread_id].page_table[lrublock.page_id].in_memory = false;
                threads[lrublock.thread_id].page_table[lrublock.page_id].disk_id = findDiskBlock(lrublock.thread_id, lrublock.page_id);
                lrublock.tim = get_current_time(), lrublock.thread_id = thread_id, lrublock.page_id = page_num;

                threads[lrublock.thread_id].page_table[lrublock.page_id]={block_id, 0, thread_id, true, get_current_time()};
            }
        }
    }
    int get_current_time()  //获取当前时间 主要是为LRU算法服务
    {
        return ++startTime;
    }
};
int MemoryManager::threadsID = 1;
int MemoryManager::startTime = 0;
