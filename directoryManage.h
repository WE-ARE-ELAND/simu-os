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
    /*用户结构体定义*/
    typedef struct User {
        string name;//用户名
        int userRight;//用户权限
        string account;//用户账号
        string password;//用户密码
        int state;//用户状态
    };
    /*文件结构体定义*/
    typedef struct File {
        string name;//文件名
        string context;//文件内容
        int size;//文件大小
        string structure;//文件结构
        int canRead;//文件读权限
        int canWrite;//文件写权限
    }File;
    typedef struct DirectoryEntry;
    /*目录结构体定义*/
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
    /*目录项（文件控制块）结构体定义*/
    typedef struct DirectoryEntry
    {
        File file;       // 文件名
        string owner; // 文件所有者
        time_t create_time;
        int first_block;//在磁盘中存放的起始地址
        //int block_num;     // 占用的盘块数
        DirectoryEntry* frontFile;//同级目录上一文件
        DirectoryEntry* nextFile;//同级目录下一文件
        Directory* parentDirectory;//父目录
    }DirectoryEntry;
public:
    void CreateRootDirectory();//创建根目录
    void MakeDir(string name);//新建目录：新建一个空目录
    void DeleteDir(string name);   //删除目录：删除目录及目录中的文件
    void ListAll();    //显示当前目录下所有文件夹
    void ModifyDir(string name,string newName);    //修改目录：重命名
    void GetPath(Directory* curDir);    //获取当前文件夹的路径
    void CreateFile(string name);    //创建文件：在此方法中输入数据大小、数据信息、存储目录、文件名等，然后调用数据生成线程来创建文件。
    void CreateDirEntry(File file,string owner,time_t create_time,int first_block);//为文件建立目录项
    void DeleteFile(string name);    //删除文件：删除文件的目录项
    void ReadFile(string name);    //读文件：可读文件时才能读
    void WriteFile(string name);   //写文件：可写文件时才能写
    void ModifyFile(string name);    //修改文件：重命名或修改文件权限
    void OpenFile(string name);    //文件的打开
    void CloseFile(string name);    //文件的关闭
private:
    Directory* workDir;//当前工作目录
    Directory* rootDir;//根目录
    string path;//路径
    User user;//用户
};
