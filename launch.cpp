/**该项目所需头文件*/
#include "directoryManage.h"
#include "diskManage.h"
#include "memoryManage.h"
#include "threadManage.h"

// Global variables
Directory directory;
std::vector<MemoryBlock> memory;
std::vector<PageTableEntry> page_table;
std::vector<std::thread> threads;

// Mutex and condition variable for synchronizing access to memory 删掉
// std::mutex memory_mutex;
// std::condition_variable memory_cv;

int main()
{
    // Initialize memory blocks
    for (int i = 0; i < NUM_MEMORY_BLOCKS; i++)
    {
        MemoryBlock block;
        block.id = i;
        block.is_free = true;
        block.data.resize(MEMORY_BLOCK_SIZE);
        memory.push_back(block);
    }

    // Initialize page table
    page_table.resize(NUM_MEMORY_BLOCKS);
    for (int i = 0; i < NUM_MEMORY_BLOCKS; i++)
    {
        page_table[i].memory_block_id = i;
        page_table[i].is_valid = false;
    }

    // Create threads
    threads.emplace_back(data_generation_thread);
    threads.emplace_back(delete_data_thread);
    threads.emplace_back(execute_thread);

    // Visualize memory state
    visualize_memory();

    // Join threads
    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}
