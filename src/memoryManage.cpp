#include "memoryManage.h"

int MemoryManager::threadsID_ = 1;
int MemoryManager::startTime_ = 0;

MemoryManager::MemoryManager()
{
    for (int i = 0; i < NUM_MEMORY_BLOCKS_; i++)
    {
        LRUblocks_[i] = {0, 0, -1, -1};
        blocks_.push_back(i);
    }
    for (int i = 0; i < DISK_END_ - DISK_START_ + 1; i++)
        disks_[i] = {0, 0};
}

int MemoryManager::findLRUblock()
{
    int maxn = INF_, choice_id = -1;
    for (int i = 0; i < NUM_MEMORY_BLOCKS_; i++)
    {
        if (LRUblocks_[i].tim < maxn)
            maxn = LRUblocks_[i].tim, choice_id = i;
    }
    return choice_id;
}

void MemoryManager::showCurrentBlocks(const File &f)
{
    std::set<int> usedBlocks;
    for (const auto &block : f.blocks)
        usedBlocks.insert(block.id);
    int blockID = 0;
    while (static_cast<int>(usedBlocks.size()) < DEFAULT_NUM_BLOCKS_ &&
           blockID < static_cast<int>(blocks_.size()))
    {
        usedBlocks.insert(blocks_[blockID]);
        blockID++;
    }
    for (int i = 0; i < NUM_MEMORY_BLOCKS_; i++)
    {
        if (i % DEFAULT_NUM_BLOCKS_ == 0)
            std::cout << std::endl;
        std::cout << (usedBlocks.count(i) ? "*" : "#");
    }
}

void MemoryManager::allocateThreads(std::string threadName, std::string content)
{
    int needBlock = (content.size() + 39) / MEMORY_BLOCK_SIZE_;
    File tmp;
    tmp.id = threadsID_++;
    tmp.name = std::move(threadName);
    fileNameIdConvert_[tmp.name] = tmp.id;

    if (needBlock <= static_cast<int>(blocks_.size()))
    {
        std::cout << "\nCurrent available memory blocks is enough to allocate\n";
        int j = static_cast<int>(blocks_.size()) - 1;
        for (int i = 0; i < needBlock; i++)
        {
            std::string nowContent = content.substr(i * MEMORY_BLOCK_SIZE_, MEMORY_BLOCK_SIZE_);
            MemoryBlock newBlock = {blocks_[j], false, std::move(nowContent)};
            tmp.blocks.push_back(newBlock);
            tmp.page_table[i] = {blocks_[j], 0, tmp.id, true, get_current_time()};
            LRUblocks_[blocks_[j]] = {1, get_current_time(), tmp.id, i};
            blocks_.pop_back();
            j--;
        }
        threads_[tmp.id] = std::move(tmp);
        showCurrentBlocks(threads_[tmp.id]);
    }
    else
    {
        std::cout << "\nWe don't have enough blocks and will use LRU algorithm to find available blocks\n";
        int j = static_cast<int>(blocks_.size()) - 1;
        int nowsize = static_cast<int>(blocks_.size());
        for (int i = 0; i < nowsize; i++)
        {
            std::string nowContent = content.substr(i * MEMORY_BLOCK_SIZE_, MEMORY_BLOCK_SIZE_);
            MemoryBlock newBlock = {blocks_[j], false, std::move(nowContent)};
            tmp.blocks.push_back(newBlock);
            tmp.page_table[i] = {blocks_[j], 0, tmp.id, true, get_current_time()};
            LRUblocks_[blocks_[j]] = {1, get_current_time(), tmp.id, i};
            blocks_.pop_back();
            j--;
        }
        std::cout << "We should throw these blocks out:\n";
        for (int i = nowsize; i < needBlock; i++)
        {
            int findBlock = findLRUblock();
            LruBlock lrublock = LRUblocks_[findBlock];
            std::cout << findBlock << "  ";

            threads_[lrublock.thread_id].page_table[lrublock.page_id].in_memory = false;
            threads_[lrublock.thread_id].page_table[lrublock.page_id].disk_id =
                findDiskBlock(lrublock.thread_id, lrublock.page_id);
            LRUblocks_[findBlock].tim = get_current_time();

            int avaBlock =
                threads_[lrublock.thread_id].page_table[lrublock.page_id].memory_block_id;
            std::string nowContent = content.substr(i * MEMORY_BLOCK_SIZE_, MEMORY_BLOCK_SIZE_);
            MemoryBlock newBlock = {avaBlock, false, std::move(nowContent)};
            tmp.blocks.push_back(newBlock);
            tmp.page_table[i] = {avaBlock, 0, tmp.id, true, get_current_time()};
        }
        threads_[tmp.id] = std::move(tmp);
        showCurrentBlocks(threads_[tmp.id]);
    }
}

void MemoryManager::showPage(std::string threadName)
{
    int fileId = fileNameIdConvert_[threadName];
    std::cout << "------------页表--------------\n";
    std::cout << "Page id            block id\n";
    for (const auto &page : threads_[fileId].page_table)
        std::cout << page.first << "                     " << page.second.memory_block_id << std::endl;
}

int MemoryManager::findDiskBlock(int thread_id, int pageid)
{
    for (int i = 0; i <= DISK_END_ - DISK_START_; i++)
    {
        if (disks_[i].first == 0)
        {
            disks_[i] = {thread_id, pageid};
            return i;
        }
    }
    return 0;
}

void MemoryManager::deleteBlock(std::string threadName)
{
    int threadID = fileNameIdConvert_[threadName];
    File thread = threads_[threadID];
    threads_.erase(threadID);
    std::cout << "\nThe memory we need to delete is as follows:\n";
    std::set<int> freed;
    for (const auto &page : thread.page_table)
    {
        if (page.second.in_memory)
        {
            freed.insert(page.second.memory_block_id);
            blocks_.push_back(page.second.memory_block_id);
            if (page.second.disk_id != 0)
                disks_[page.second.disk_id].first = 0;
        }
    }
    for (int i = 0; i < NUM_MEMORY_BLOCKS_; i++)
    {
        if (i % DEFAULT_NUM_BLOCKS_ == 0)
            std::cout << std::endl;
        std::cout << (freed.count(i) ? "*" : "#");
    }
}

int MemoryManager::get_current_time()
{
    return ++startTime_;
}
