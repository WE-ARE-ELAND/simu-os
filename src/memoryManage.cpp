#include "memoryManage.h"

int MemoryManager::threadsID_ = 1;
int MemoryManager::startTime_ = 0;

MemoryManager::MemoryManager(DiskManager& diskRef) : diskRef_(diskRef)
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
        if (LRUblocks_[i].in_memory == 1 && LRUblocks_[i].tim < maxn)
            maxn = LRUblocks_[i].tim, choice_id = i;
    }
    return choice_id;
}

void MemoryManager::showCurrentBlocks(const File &f) const
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

void MemoryManager::allocateThreads(const std::string& threadName, const std::string& content)
{
    int needBlock = (content.size() + 39) / MEMORY_BLOCK_SIZE_;
    File tmp;
    tmp.id = threadsID_++;
    tmp.name = threadName;
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
            if (findBlock < 0)
            {
                std::cout << "No evictable page available.\n";
                break;
            }
            LruBlock lrublock = LRUblocks_[findBlock];
            std::cout << findBlock << "  ";

            auto& evicted = threads_[lrublock.thread_id];
            std::string evictedData = evicted.blocks[lrublock.page_id].data;
            int diskOffset = findDiskBlock(lrublock.thread_id, lrublock.page_id);
            evicted.page_table[lrublock.page_id].in_memory = false;
            evicted.page_table[lrublock.page_id].disk_id = diskOffset;
            diskRef_.writeSwapBlock(static_cast<short>(DISK_START_ + diskOffset), evictedData);

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

void MemoryManager::showPage(const std::string& threadName) const
{
    int fileId = fileNameIdConvert_.at(threadName);
    std::cout << "------------ Page Table ------------\n";
    std::cout << "Page id            block id\n";
    for (const auto &page : threads_.at(fileId).page_table)
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

void MemoryManager::pageOut(int thread_id, int page_id)
{
    auto it = threads_[thread_id].page_table.find(page_id);
    if (it == threads_[thread_id].page_table.end()) return;
    auto& page = it->second;
    if (!page.in_memory) return;
    int diskOffset = findDiskBlock(thread_id, page_id);
    std::string data = threads_[thread_id].blocks[page_id].data;
    diskRef_.writeSwapBlock(static_cast<short>(DISK_START_ + diskOffset), data);
    page.disk_id = diskOffset;
    page.in_memory = false;
}

void MemoryManager::pageIn(int thread_id, int page_id)
{
    auto it = threads_[thread_id].page_table.find(page_id);
    if (it == threads_[thread_id].page_table.end()) return;
    auto& page = it->second;
    if (page.in_memory) return;
    std::string data;
    diskRef_.readSwapBlock(static_cast<short>(DISK_START_ + page.disk_id), data);
    int block = findLRUblock();
    if (block < 0 && !blocks_.empty()) { block = blocks_.back(); blocks_.pop_back(); }
    if (block < 0) return;
    // Evict current occupant of block
    if (LRUblocks_[block].in_memory == 1)
    {
        int evictThread = LRUblocks_[block].thread_id;
        int evictPage = LRUblocks_[block].page_id;
        if (evictThread >= 0 && evictThread < static_cast<int>(threads_.size()))
        {
            int diskOffset = findDiskBlock(evictThread, evictPage);
            diskRef_.writeSwapBlock(static_cast<short>(DISK_START_ + diskOffset),
                threads_[evictThread].blocks[evictPage].data);
            auto pit = threads_[evictThread].page_table.find(evictPage);
            if (pit != threads_[evictThread].page_table.end())
            {
                pit->second.disk_id = diskOffset;
                pit->second.in_memory = false;
            }
        }
    }
    threads_[thread_id].blocks[page_id].data = data;
    threads_[thread_id].blocks[page_id].id = block;
    page.memory_block_id = block;
    page.in_memory = true;
    LRUblocks_[block] = {1, get_current_time(), thread_id, page_id};
}

void MemoryManager::deleteBlock(const std::string& threadName)
{
    int threadID = fileNameIdConvert_[threadName];
    File thread = std::move(threads_[threadID]);
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