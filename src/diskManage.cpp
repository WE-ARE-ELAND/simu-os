
#include "diskManager.h"

DiskManager::DiskManager()
{
    FreeDataBlockNum_ = NUM_BLOCKS_ - NUM_FAT_BLOCKS_; // 前52块用于存储FAT表
    FreeSwapBlockNum_ = NUM_SWAP_BLOCKS_;
    init();
}

void DiskManager::init()
{
    initDisk();
    initFat();
    initFreeBlocks();
}

void DiskManager::initDisk()
{
    for (int i = 0; i < NUM_BLOCKS_; i++)
    {
        MyDisk_[i].BlockNum = i;
        MyDisk_[i].data = std::string(BLOCK_SIZE, '#');
    }
}

void DiskManager::initFat()
{
    for (int i = 0; i < NUM_BLOCKS_; ++i)
    {
        fatList_[i] = static_cast<short>(-2);
    }
}

void DiskManager::initFreeBlocks()
{
    int num_block = NUM_FAT_BLOCKS_; // 空闲块号从52到1023
    for (int i = 0; i < NUM_FREE_GROUPS_; ++i)
    {
        if (i == NUM_FREE_GROUPS_ - 1)
        {
            freeBlocks_[i][0] = NUM_BLOCKS_ - NUM_FAT_BLOCKS_ - (NUM_FREE_PER_GROUP_ * (NUM_FREE_GROUPS_ - 1));
            freeBlocks_[i][1] = 0;
        }
        else
        {
            freeBlocks_[i][0] = NUM_FREE_PER_GROUP_;
            freeBlocks_[i][1] = i + 1;
        }
        for (int j = 2; j < NUM_FREE_PER_GROUP_ + 2; ++j)
        {
            freeBlocks_[i][j] = num_block;
            num_block++;
            if (num_block == NUM_BLOCKS_)
                break;
        }
        if (num_block == NUM_BLOCKS_)
            break;
    }
}

/**
 * 利用成组连接法分配空闲块
 * @return 分配的磁盘块块号，若为-1表示分配失败
 */
short DiskManager::allocateBlock()
{
    int freeNum;
    short allocNum;
    if (freeBlocks_[0][0] > 0)
    {
        freeNum = freeBlocks_[0][0];
        allocNum = freeBlocks_[0][freeNum + 1];
        freeBlocks_[0][0]--;
        if (allocNum < NUM_FILE_BLOCKS_)
        {
            FreeDataBlockNum_--;
        }
        else
        {
            FreeSwapBlockNum_--;
        }
    }
    else
    {
        int nextGroup = freeBlocks_[0][1];
        if (nextGroup != 0)
        {
            for (int j = 0; j < freeBlocks_[0][0] + 2; j++)
            {
                freeBlocks_[0][j] = freeBlocks_[nextGroup][j];
            }
            freeNum = freeBlocks_[0][0];
            allocNum = freeBlocks_[0][freeNum + 1];
            freeBlocks_[0][0]--;
            if (allocNum < NUM_FILE_BLOCKS_)
            {
                FreeDataBlockNum_--;
            }
            else
            {
                FreeSwapBlockNum_--;
            }
        }
        else
        {
            return -1;
        }
    }
    return allocNum;
}

void DiskManager::writeBlock(short num_block, std::string data)
{
    MyDisk_[num_block].data = std::move(data);
}

std::string DiskManager::readBlock(short block_num)
{
    return MyDisk_[block_num].data;
}

int DiskManager::freeBlock(short num_block)
{
    MyDisk_[num_block].data = std::string(BLOCK_SIZE, '#');
    if (fatList_[num_block] == -2)
    {
        return -1;
    }
    int k = 0;
    while (freeBlocks_[k][0] == NUM_FREE_PER_GROUP_)
    {
        k = freeBlocks_[k][1];
    }
    freeBlocks_[k][freeBlocks_[k][0] + 2] = num_block;
    freeBlocks_[k][0]++;
    fatList_[num_block] = -2;
    if (num_block < NUM_FILE_BLOCKS_)
    {
        FreeDataBlockNum_++;
    }
    else
    {
        FreeSwapBlockNum_++;
    }
    return 1;
}

int DiskManager::AllocateBlocks(std::string fileName, int size, std::string data)
{
    if (size > FreeDataBlockNum_)
    {
        return -1;
    }
    short num_block, tmp_num_block;
    int index = 0;
    while (size--)
    {
        num_block = allocateBlock();
        if (index < static_cast<int>(data.size()))
        {
            if (static_cast<int>(data.size()) - index < BLOCK_SIZE)
            {
                writeBlock(num_block, data.substr(index, data.size() - index));
            }
            else
            {
                writeBlock(num_block, data.substr(index, BLOCK_SIZE));
            }
        }
        if (index == 0)
        {
            fileNameToNumOfBlock_[fileName] = num_block;
        }
        else
        {
            fatList_[tmp_num_block] = num_block;
        }
        tmp_num_block = num_block;
        index += BLOCK_SIZE;
    }
    fatList_[num_block] = -1;
    return 1;
}

void DiskManager::DeallocateBlocks(std::string fileName)
{
    short start_num_block = fileNameToNumOfBlock_[fileName];
    short num_block = start_num_block;
    while (fatList_[num_block] != -1)
    {
        int next_num_block = fatList_[num_block];
        freeBlock(num_block);
        fatList_[num_block] = -2;
        num_block = next_num_block;
    }
    freeBlock(num_block);
    fatList_[num_block] = -2;
    fileNameToNumOfBlock_.erase(fileName);
}

void DiskManager::writeSwapBlock(short blockNum, std::string buffer)
{
    MyDisk_[blockNum].data = std::move(buffer);
}

void DiskManager::readSwapBlock(short blockNum, std::string &buffer)
{
    buffer = MyDisk_[blockNum].data; // Bug 6 fix
}

std::string DiskManager::ReadFileDataFromDisk(std::string fileName)
{
    int start_num_block = fileNameToNumOfBlock_[fileName];
    std::string data;
    int num_block = start_num_block;
    while (fatList_[num_block] != -1)
    {
        data.append("盘块" + std::to_string(num_block) + ":" + readBlock(num_block) + "\n");
        num_block = fatList_[num_block];
    }
    data.append("盘块" + std::to_string(num_block) + ":" + readBlock(num_block) + "\n");
    return data;
}

void DiskManager::PrintMyDisk()
{
    for (int i = 0; i < NUM_BLOCKS_; i++)
    {
        std::cout << MyDisk_[i].data << std::endl;
    }
}

void DiskManager::dumpFile()
{
    std::string filename("disk.txt");
    std::fstream output_fstream(filename, std::ios_base::out | std::ios_base::binary);
    if (!output_fstream.is_open())
    {
        std::cerr << "Failed to open " << filename << '\n';
        return;
    }
    for (int i = 0; i < NUM_BLOCKS_; ++i)
    {
        output_fstream << MyDisk_[i].data;
    }
    output_fstream.close();
}

void DiskManager::printFreeBlocks()
{
    for (int i = 0; i < freeBlocks_[0][0] + 2; ++i)
    {
        if (i == 0)
            std::cout << "该组空闲块数：";
        if (i == 1)
            std::cout << "下一组编号：";
        if (i == 2)
            std::cout << std::endl << "空闲块编号：" << std::endl;
        if (i > 2 && (i - 2) % 10 == 0)
            std::cout << std::endl;
        std::cout << freeBlocks_[0][i] << " \t";
    }
    std::cout << std::endl;
}

std::vector<int> DiskManager::getAllBlocks(std::string fileName)
{
    std::vector<int> blocks;
    int start_num_block = fileNameToNumOfBlock_[fileName];
    int num_block = start_num_block;
    while (fatList_[num_block] != -1)
    {
        blocks.push_back(num_block);
        num_block = fatList_[num_block];
    }
    blocks.push_back(num_block);
    return blocks;
}
