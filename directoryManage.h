#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <queue>
#include <stack>
using namespace std;

class DirectoryManage
{
    typedef struct User {
        string name;
        int userRight;
        string account;
        string password;
        int state;
    };
    typedef struct File {
        string name;//文件名
        string context;//文件内容
        int size;//文件大小
        string structure;//文件结构
        time_t create_time;//文件创建时间
        string owner; // 文件所有者
        int canRead;//文件读权限
        int canWrite;//文件写权限
    }File;
    typedef struct DirectoryEntry;
    typedef struct Directory
    {
        string name;//目录名
        Directory* nextDirectory;//同级下一目录
        Directory* frontDirectory;//同级上一目录
        Directory* parentDirectory;//父目录
        Directory* firstChildDirectory;//子目录
        DirectoryEntry* firstChildFile;//首个子文件
        DirectoryEntry* lastChildFile;//最后一个子文件
        int canRead;//读权限，0代表不可读，1代表最高权限
        int canWrite;//写权限，0代表不可写，1代表最高权限
    }Directory;
    typedef struct DirectoryEntry//目录项（文件控制块）
    {
        File file;       // 文件
        int first_block;//在磁盘中存放的起始地址
        DirectoryEntry* frontFile;//同级目录上一文件
        DirectoryEntry* nextFile;//同级目录下一文件
        Directory* parentDirectory;//父目录
    }DirectoryEntry;
public:
    void CreateRootDirectory();//创建根目录

    void tree(Directory* curDir,int level);//显示当前目录的树形结构

    void MakeDir(string name);//md新建目录：新建一个空目录

    void cdDir(string path);//cd切换目录

    void rd(string name);//rd删除目录
    void DeleteDir(Directory* p);   //删除目录中的所有文件及文件夹

    void dir();//dir显示目录及文件信息
    void ListDir(Directory* p);//显示某子目录的信息
    void ListAll();    //显示当前目录下所有文件夹
    void dirFile(string name);//显示当前目录下某文件的信息
    void dirPath(string path);//显示相对路径下某子目录的信息

    void ren(string name, string newName);//ren重命名目录或者文件
    void RenameDir(Directory* ptr,string newName);    //重命名目录
    void RenameFile(DirectoryEntry* ptr, string newName);    //重命名文件

    string GetPath(Directory* curDir);    //获取当前文件夹的路径

    void CreateFile(string name);    //create创建文件
    void CreateDirEntry(File file,int first_block);//为文件建立目录项

    void DeleteFile(string name);    //del删除文件：删除文件的目录项

    void ReadFile(string path);    //type读文件，可读文件时才能读

    void WriteFile(string name);   //echo写文件，可写文件时才能写
    
    void OpenFile(string name);    //open文件的打开
    void CloseFile(string name);    //close文件的关闭
public:
    Directory* workDir;//当前工作目录
    Directory* rootDir;//根目录
    string path;//路径
    User user;//用户
};

