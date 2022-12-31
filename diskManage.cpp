
#include "DiskManager.h"

using namespace std;

/**
 * 构造函数
 * 初始化磁盘、FAT表和空闲磁盘表
 */
DiskManager::DiskManager()
{
    FreeDataBlockNum = 900 - 52; // 前52块用于存储fat表
    FreeSwapBlockNum = 124;
    init(); // 初始化磁盘、FAT表和空闲磁盘表
}

/**
 * 初始化数据结构
 */
void DiskManager::init()
{
    // 初始化磁盘
    initDisk();
    // 初始化fat表
    initFat();
    // 初始化空闲盘块
    initFreeBlocks();
}
/**
 * 初始化磁盘数据
 */
void DiskManager::initDisk()
{
    for (int i = 0; i < 1024; i++)
    {
        MyDisk[i].BlockNum = i;
        // 前52个磁盘块用于存储FAT表
        MyDisk[i].data = "########################################"; // 40个字符，正好40字节
    }
}
/**
 * 初始化fat表
 */
void DiskManager::initFat()
{
    for (int i = 0; i < 1024; ++i)
    {
        fatList[i] = (short)-2;
    }
}
/**
 * 初始化空闲盘块
 */
void DiskManager::initFreeBlocks()
{
    // 将空闲盘块写入freeBlocks
    // 100个一组，数组第一个位置存该组现存空闲块的数量
    int num_block = 52; // 空闲块号从52到1023
    // 将空闲盘块写入freeBlocks
    // 100个一组，数组第一个位置存该组现存空闲块的数量，第二个位置存储下一个磁盘块编号/是否存在下一个磁盘块
    for (int i = 0; i < 10; ++i)
    {
        if (i == 9)
        {
            freeBlocks[i][0] = 72;
            freeBlocks[i][1] = 0;
        }
        else
        {
            freeBlocks[i][0] = 100;
            freeBlocks[i][1] = i + 1;
        }
        for (int j = 2; j < 102; ++j)
        {
            freeBlocks[i][j] = num_block;
            num_block++;
            if (num_block == 1024)
            {
                break;
            }
        }
        if (num_block == 1024)
        {
            break;
        }
    }
}

/**
 * 创建实例
 */
DiskManager *DiskManager::getInstance()
{
    static DiskManager *instance;
    if (instance == nullptr)
    {
        instance = new DiskManager();
    }
    return instance;
}

/** 最微操作：
 * 1. 分配一块磁盘块；
 * 2. 向某磁盘块写入信息；
 * 3. 读取某磁盘块信息；
 * 4. 释放某磁盘块。
 */

/**
 * 利用成组连接法分配空闲块
 * @return 分配的磁盘块块号，若为-1表示分配失败
 */
short DiskManager::allocateBlock()
{
    // 空闲块数,分配的盘块号
    int freeNum;
    short allocNum;
    if (freeBlocks[0][0] > 0)
    {
        freeNum = freeBlocks[0][0];
        allocNum = freeBlocks[0][freeNum + 1];
        fatList[allocNum] = -1;
        freeBlocks[0][0]--;
        if (allocNum < 900)
        {
            FreeDataBlockNum--;
        }
        else
        {
            FreeSwapBlockNum--;
        }
    }
    else
    {
        // 不是链尾，还有其它空闲块组
        if (freeBlocks[0][1] != 0)
        {
            // 当前组已经分配完，下一组拷贝到当前组
            for (int j = 0; j < freeBlocks[freeBlocks[0][1]][0]; j++)
            {
                freeBlocks[0][j] = freeBlocks[freeBlocks[0][1]][j];
            }
            freeNum = freeBlocks[0][0];
            allocNum = freeBlocks[0][freeNum + 1];
            fatList[allocNum] = -1;
            freeBlocks[0][0]--;
            if (allocNum < 900)
            {
                FreeDataBlockNum--;
            }
            else
            {
                FreeSwapBlockNum--;
            }
        }
        else
        {
            return -1;
        }
    }
    return allocNum;
}

/**
 * 将信息写入指定磁盘块
 * @param num_block 磁盘块块号
 * @param data 需要写入的数据，目前要求是字符型
 */
void DiskManager::writeBlock(short num_block, string data)
{
    MyDisk[num_block].data = data;
    //     fstream fs("Disk.txt", ios::binary | ios::out);
    //     fs.seekp(, ios::beg);
    //     fs.write("!!!", 3);
    //     fs.close();
}

/**
 * 读取指定磁盘块的信息
 * @param num_block 磁盘块块号
 * @return data 读取的数据
 */
string DiskManager::readBlock(short block_num)
{
    return MyDisk[block_num].data;
}

/**
 * 释放指定磁盘块空间
 * todo 修改成组连接法的数据结构
 * @param num_block 磁盘块块号
 */
int DiskManager::freeBlock(short num_block)
{
    // 1. 删除数据
    MyDisk[num_block].data = "########################################";
    // todo 2. 修改成组连接法的数据结构
    if (fatList[num_block] == -2)
    {
        return -1;
    }
    int k = 0;
    while (freeBlocks[k][0] == 100)
    {
        k = freeBlocks[k][1];
    }
    freeBlocks[k][freeBlocks[k][0] + 2] = num_block;
    freeBlocks[k][0]++;
    cout << k;
    fatList[num_block] = -2;
    // 3. 修改空闲块数
    if (num_block < 900)
    {
        FreeDataBlockNum++;
    }
    else
    {
        FreeSwapBlockNum++;
    }
    return 1;
}

/***************************************************************/

/**
 * 存储文件，为文件分配磁盘块
 * @param fileName 文件名
 * @param size 所需的磁盘块数量
 * @param data 写入的数据
 * @return 空间是否充足
 */
int DiskManager::AllocateBlocks(string fileName, int size, string data)
{
    // 1. 空闲磁盘块不足以存储相应大小的文件
    if (size > FreeSwapBlockNum)
    {
        return -1;
    }
    // 2. 分配相应大小的磁盘块
    short num_block, tmp_num_block; // 动态分配的块号, 上一个分配的磁盘块
    int index = 0;                  // 索引号；
    while (size--)
    {
        // 2.1 分配磁盘块
        num_block = allocateBlock();
        if (index < data.size())
        {
            if (data.size() < 16)
            {
                writeBlock(num_block, data.substr(index, data.size()));
            }
            else
            {
                writeBlock(num_block, data.substr(index, 16));
            }
        }
        // 2.2 将分配的盘块号写入FAT表
        {
            if (index == 0)
            {
                fileNameToNumOfBlock[fileName] = num_block;
            }
            else
            {
                fatList[tmp_num_block] = num_block;
            }
            tmp_num_block = num_block;
        }
        index += 16;
    }
    fatList[num_block] = -1;
    return 1;
}

/**
 * 删除文件，回收空闲块
 * @param fileName 文件名
 */
void DiskManager::DeallocateBlocks(string fileName)
{
    int start_num_block = fileNameToNumOfBlock[fileName];
    // 1. 更新FAT表
    int num_block = start_num_block;
    while (fatList[num_block] != -1)
    {
        int next_num_block = fatList[num_block];
        fatList[num_block] = -2;
        {
            // 2. 更新空闲盘块
            freeBlock(num_block);
        }
        num_block = next_num_block;
    }
    fatList[num_block] = -2;
    freeBlock(num_block);
    fileNameToNumOfBlock.erase(fileName);
}

/**
 * todo 保存数据到交换区, 可以指定位置吗？
 * @param data 内存中的数据
 */
void DiskManager::writeSwapBlock(short blockNum, string buffer)
{
    MyDisk[blockNum].data = buffer;
}

void DiskManager::readSwapBlock(short blockNum, string &buffer)
{
    buffer = MyDisk[blockNum].data[0];
}

/**
 * 读取文件数据
 * @param fileName 文件名
 * @param size 文件所占的磁盘块数量
 * @return 文件数据
 */
string DiskManager::ReadFileDataFromDisk(string fileName)
{
    int start_num_block = fileNameToNumOfBlock[fileName];
    string data = "";
    int num_block = start_num_block;
    while (fatList[num_block] != -1)
    {
        data.append(readBlock(num_block));
        num_block = fatList[num_block];
    }
    data.append(readBlock(num_block));
    return data;
}

/**
 * 打印磁盘数据
 */
void DiskManager::PrintMyDisk()
{
    for (int i = 1; i <= 1024; i++)
    {
        cout << MyDisk[i - 1].data << endl;
    }
    // dumpFile();
};

/**
 * 将磁盘数据写入物理文件
 */
void DiskManager::dumpFile()
{
    string filename("disk.txt");
    fstream output_fstream;

    output_fstream.open(filename, std::ios_base::out);
    if (!output_fstream.is_open())
    {
        cerr << "Failed to open " << filename << '\n';
    }
    else
    {
        for (int i = 0; i < 1024; ++i)
        {
            output_fstream << MyDisk[i].data;
        }
    }
}

/**
 * 打印空闲盘块状态
 */
void DiskManager::printFreeBlocks()
{
    for (int i = 0; i < freeBlocks[0][0] + 2; ++i)
    {
        if (i == 2)
        {
            cout << endl;
        }
        if (i > 2 && (i - 2) % 10 == 0)
        {
            cout << endl;
        }
        cout << freeBlocks[0][i] << " ";
    }
    cout << endl;
}

/**
 * 测试代码
 */
// int main()
// {
//     // DiskManager DIM = getInstance();
//     short num_block = DiskManager::getInstance()->allocateBlock();
//     DiskManager::getInstance()->writeBlock(num_block, "hello world");
//     cout << DiskManager::getInstance()->readBlock(num_block);
//     cout << DiskManager::getInstance()->fatList[num_block];
//     return 0;
// }