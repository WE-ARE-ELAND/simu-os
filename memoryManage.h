#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>

/**申请一块2560B的内存空间模拟内存，按逻辑划分为64块，每块40B。
 * 将目录中选中的文件读入内存，并显示文件中信息（字符）。
 * 内存可以同时存放多个文件信息，每个文件固定分配8个内存块*/

const int MEMORY_SIZE = 2560;     // Total size of the memory in bytes
const int MEMORY_BLOCK_SIZE = 40; // Size of each memory block in bytes
const int NUM_MEMORY_BLOCKS = 64; // Number of memory blocks
const int DEFAULT_NUM_BLOCKS = 8; // Number of blocks assigned to each thread by default

// Data structure to represent a memory block
struct MemoryBlock
{
    int id;            // Unique identifier for the block
    int size;          // Size of the block in bytes
    bool is_free;      // Flag to indicate if the block is free or in use
    vector<char> data; // Data in the block
};

// Data structure to represent a page table entry 页表项
struct PageTableEntry
{
    int memory_block_id;  // ID of the memory block
    int block_id;         // 磁盘块id
    int thread_id;        // ID of the thread using the block
    int access_count;     // Number of times the block has been accessed
    bool is_valid;        // Flag to indicate if the entry is valid or not
    bool in_memory;       // 表示页面是否在内存中（如果不在内存中，需要先调入内存）
    bool modify_bit;      // 修改位（Modify Bit）：表示该页是否被修改过。
    int last_access_time; // 最后一次被访问的时间（用于实现LRU算法）
};

// Data structure to represent a thread （PCB）
struct Thread
{
    int id;                                             // Unique identifier for the thread
    std::string name;                                   // Name of the thread
    std::vector<int> blocks;                            // List of memory blocks assigned to the thread
    std::unordered_map<int, PageTableEntry> page_table; // Page table for the thread 页表： 页号->页表项
};

// Memory management class
class MemoryManager
{
private:
    std::vector<MemoryBlock> blocks;         // List of memory blocks
    std::unordered_map<int, Thread> threads; // Map of threads indexed by their IDs
    int num_free_blocks;                     // Number of free blocks in the memory

    // Private helper function to find a free block of a given size
    int findFreeBlock(int size)
    {
        // Iterate through the list of blocks
        for (int i = 0; i < NUM_MEMORY_BLOCKS; i++)
        {
            // Check if the current block is free and has enough size
            if (blocks[i].is_free && blocks[i].size >= size)
            {
                return i; // Return the block if found
            }
        }
        return -1; // Return -1 if no free block is found
    }

    // 内存回收的LRU算法
    //  Private helper function to find the least recently used block for a given thread
    int findLRUBlock(Thread &thread)
    {
        int lru_block_id = -1;          // ID of the least recently used block
        int min_access_count = INT_MAX; // Minimum access count

        // Iterate through the page table of the thread
        for (auto &entry : thread.page_table)
        {
            // Check if the current entry is valid and has a smaller access count
            if (entry.second.is_valid && entry.second.access_count < min_access_count)
            {
                lru_block_id = entry.first;                   // Update the least recently used block ID
                min_access_count = entry.second.access_count; // Update the minimum access count
            }
        }
        return lru_block_id; // Return
    }
};

/** 以下代码为LRU算法的实例代码，不能直接使用，需要修改！*/
const int NUM_PAGES = 10; // 页表中的页面数量
// 页表
// PageTableEntry page_table[NUM_PAGES];

// 获取当前时间
int get_current_time()
{
    return (int)time(NULL);
}

// 模拟访问某个页面
void access_page(int page_num)
{

    // 如果页面不在内存中，则将其调入内存
    if (!page_table[page_num].in_memory)
    {
        // TODO：如果内存中有空闲块，则为进程分配一个空闲块，修改相应表项
        // 找到内存中最近最少使用的页面

        // 如果没有空闲块，执行以下代码：
        int oldest_page = 0;
        for (int i = 1; i < NUM_PAGES; i++)
        {
            if (page_table[i].last_access_time < page_table[oldest_page].last_access_time)
            {
                oldest_page = i;
            }
        }
        // 将最近最少使用的页面调出内存
        page_table[oldest_page].in_memory = false;
        // 将新页面调入内存
        page_table[page_num].in_memory = true;
    }
    // 更新页面的最后访问时间
    page_table[page_num].last_access_time = get_current_time();
}
/** LRU算法实例代码结束*/