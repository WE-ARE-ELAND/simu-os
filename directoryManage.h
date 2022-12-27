#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
using namespace std;
// 定义目录项
struct DirectoryEntry
{
    string name;       // 文件名
    bool is_directory; // 是否是目录
    int block_num;     // 占用的盘块数
};

struct Directory
{
    string name;  // 文件名
    string owner; // 文件所有者
    vector<string> files;
    vector<Directory> subdirectories;
};

class File
{
public:
    // 文件名
    string name;
    // 文件大小（以字节为单位）
    int size;
    // 文件所有者
    string owner;
    // 创建时间
    time_t create_time;
    // 文件结构（可以为"text"、"binary"等）
    string file_structure;
    // 文件在磁盘中的地址
    vector<int> blocks;
    // 文件的数据
    vector<char> data;

    // 构造函数
    File(string name, int size, string owner, string create_time, string file_structure, int disk_address);
};
