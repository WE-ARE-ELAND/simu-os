#include "memoryManage.h"

int MemoryManager::threadsID = 1;
int MemoryManager::startTime = 0;

MemoryManager::MemoryManager()
{

    for (int i = 0; i < NUM_MEMORY_BLOCKS; i++) // 初始化空的容器
    {
        LRUblocks[i] = {0, 0, -1, -1};
        blocks.push_back(i);
        fakeVisit[i] = 0;
    }
    for (int i = 0; i < 200; i++)
        disks[i] = {0, 0};
}

int MemoryManager::findLRUblock()
{
    int maxn = INF, choice_id = -1;
    for (int i = 0; i < NUM_MEMORY_BLOCKS; i++)
    {
        if (LRUblocks[i].tim < maxn)
            maxn = LRUblocks[i].tim, choice_id = i;
    }
    return choice_id;
}

void MemoryManager::showCurrentBlocks(File &f)
{
    vector<int> v;
    for (auto &block : f.blocks)
        v.push_back(block.id),fakeVisit[block.id]=1;
    int blockID = 0;
    while(v.size()<8 && blockID < blocks.size())
    {
        if(fakeVisit[blocks[blockID]] == 0)
        {
            v.push_back(blocks[blockID]);
            fakeVisit[blocks[blockID]] = 1;
        }
        blockID ++;
    }
    set<int> s;
    for (auto& it:v)
        s.insert(it);
    for (int i = 0; i < NUM_MEMORY_BLOCKS; i++)
    {
        if (i % 8 == 0)
            printf("\n");
        if (s.count(i))
            cout << "*";
        else
            printf("#");
    }
}


void MemoryManager::allocateThreads(string threadName, string content)
{
    int needBlock = (content.size()+39)/40;
    File tmp;
    tmp.id = threadsID++, tmp.name = threadName;
    fileNameIdConvert[threadName] = tmp.id;
    if (needBlock <= blocks.size()) // 空闲块够分配
    {
        printf("\nCurrent availavle memory blocks is enough to allocate\n");
        int j = blocks.size() - 1;
        for (int i = 0; i < needBlock; i++)
        {
            string nowContent;
            if(i==needBlock-1)
                nowContent = content.substr(i*40,40);
            else 
                nowContent = content.substr(i*40);
            MemoryBlock newBlock = {blocks[j], false, nowContent}; // 找到空闲内存
            tmp.blocks.push_back(newBlock);                        // 填入相关信息
            tmp.page_table[i] = PageTableEntry{blocks[j], 0, tmp.id, true, get_current_time()};
            LRUblocks[blocks[j]] = {1, get_current_time(), tmp.id, i}; // 记录最后一次出现的时间和是否空闲
            blocks.pop_back(), j--;                                    // 弹出记录的空闲内存
        }
        threads[tmp.id] = tmp;
        showCurrentBlocks(tmp);
    }
    else // 不够分配
    /*
    1 把当前空闲块分配完    2 根据lru算法寻找出现时间最早的    3 将内存块调出内存 放入磁盘
     */
    {
        printf("\nWe don't have enough blocks and will use LRU algrothirm to find available blocks\n");
        File tmp;
        tmp.id = threadsID++, tmp.name = threadName;
        int j = blocks.size() - 1, nowsize = blocks.size();
        for (int i = 0; i < nowsize; i++)
        {
            string nowContent = content.substr(i*40);
            MemoryBlock newBlock = {blocks[j], false, nowContent}; // 找到空闲内存
            tmp.blocks.push_back(newBlock);                        // 填入相关信息
            tmp.page_table[i] = PageTableEntry{blocks[j], 0, tmp.id, true, get_current_time()};
            LRUblocks[blocks[j]] = {1, get_current_time(), tmp.id, i}; // 记录最后一次出现的时间和是否空闲
            blocks.pop_back(), j--;                                    // 弹出记录的空闲内存
        }
        printf("We should throw these blocks out:\n");
        for (int i = nowsize; i < needBlock; i++)
        {
            int findBlock = findLRUblock(); // 需要把这个块置换出去
            LruBlock lrublock = LRUblocks[findBlock];
            printf("%d  ", findBlock);

            threads[lrublock.thread_id].page_table[lrublock.page_id].in_memory = 0;
            threads[lrublock.thread_id].page_table[lrublock.page_id].disk_id = findDiskBlock(threads[lrublock.thread_id].page_table[lrublock.page_id].thread_id, threads[lrublock.thread_id].page_table[lrublock.page_id].memory_block_id);
            LRUblocks[findBlock].tim = get_current_time();

            int avaBlock = threads[lrublock.thread_id].page_table[lrublock.page_id].memory_block_id;
            string nowContent;
            if(i==needBlock-1)
                nowContent = content.substr(i*40,40);
            else 
                nowContent = content.substr(i*40);
            MemoryBlock newBlock = {avaBlock, false, nowContent};
            tmp.blocks.push_back(newBlock);
            tmp.page_table[i] = PageTableEntry{avaBlock, 0, tmp.id, true, get_current_time()};
        }
        threads[tmp.id] = tmp;
        showCurrentBlocks(tmp);
    }
}
void MemoryManager::showPage(string threadName)
{
    int fileId=fileNameIdConvert[threadName];
    printf("*****************************");
        printf("Page id            block id\n");
    for (auto&page:threads[fileId].page_table)
        printf("%d                     %d\n",page.first,page.second.memory_block_id);
}
int MemoryManager::findDiskBlock(int thread_id, int pageid)
{
    for (int i = 0; i <= diskEnd - diskStart; i++)
    {
        if (disks[i].first == 0)
        {
            disks[i] = {thread_id, pageid};
            return i;
        }
    }
    return 0;
}

void MemoryManager::deleteBlock(string threadName)
{
    int threadID = fileNameIdConvert[threadName];
    File thread = threads[threadID];
    threads.erase(threadID);
    printf("\nThe memory we need to delete is as follows:\n");
    set <int> s;
    for (auto &page : thread.page_table)
    {
        if (page.second.in_memory) // 在内存中就释放
        {
            s.insert(page.second.memory_block_id);
            fakeVisit[page.second.memory_block_id] = 0;
            // printf("%d ", page.second.memory_block_id);
            blocks.push_back(page.second.memory_block_id);
            disks[page.second.disk_id].first = 0;
        }
    }
    for (int i = 0; i < NUM_MEMORY_BLOCKS; i++)
    {
        if (i % 8 == 0)
            printf("\n");
        if (s.count(i))
            cout << "*";
        else
            printf("#");
    }
}


void MemoryManager::acessPage(int thread_id, int page_num)
{
    File thread = threads[thread_id];
    if (thread.page_table[page_num].in_memory)
    {
        LRUblocks[thread.page_table[page_num].memory_block_id].tim = get_current_time();
    }
    else
    {
        // 1 还有空闲的内存
        if (blocks.size() > 0)
        {
            int j = blocks.size() - 1, blockId = blocks[j];
            j--, blocks.pop_back();
            for (int i = 0; i <= diskEnd - diskStart; i++)
            {
                if (disks[i].first == thread_id && disks[i].second == page_num)
                {
                    disks[i].first = 0;
                    break;
                }
            }
            LRUblocks[blockId] = {true, get_current_time(), thread_id, page_num};
            PageTableEntry &now_page = threads[thread_id].page_table[page_num];
            now_page = {blockId, 0, thread_id, true, get_current_time()};
        }
        else // 无空闲内存 需要用到页面置换算法
        {
            int block_id = findLRUblock(); // 置换的内存块
            LruBlock &lrublock = LRUblocks[block_id];
            threads[lrublock.thread_id].page_table[lrublock.page_id].in_memory = false;
            threads[lrublock.thread_id].page_table[lrublock.page_id].disk_id = findDiskBlock(lrublock.thread_id, lrublock.page_id);
            lrublock.tim = get_current_time(), lrublock.thread_id = thread_id, lrublock.page_id = page_num;

            threads[lrublock.thread_id].page_table[lrublock.page_id] = {block_id, 0, thread_id, true, get_current_time()};
        }
    }
}

int MemoryManager::get_current_time()
{
    return ++startTime;
}
