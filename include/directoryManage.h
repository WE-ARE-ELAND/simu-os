#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <queue>
#include <stack>
#include <memory>
#include <ctime>

class DirectoryManage
{
public:
    struct User
    {
        std::string name;
        int userRight;
        std::string account;
        std::string password;
        int state;
    };

    struct File
    {
        std::string name;
        std::string context;
        int size;
        std::string structure;
        std::time_t create_time;
        std::string owner;
        int canRead;
        int canWrite;
    };

    struct Directory;

    struct DirectoryEntry
    {
        File file;
        int first_block;
        DirectoryEntry* frontFile = nullptr;
        std::unique_ptr<DirectoryEntry> nextFile;
        Directory* parentDirectory = nullptr;
    };

    struct Directory
    {
        std::string name;
        std::unique_ptr<Directory> nextDirectory;
        Directory* frontDirectory = nullptr;
        Directory* parentDirectory = nullptr;
        std::unique_ptr<Directory> firstChildDirectory;
        std::unique_ptr<DirectoryEntry> firstChildFile;
        DirectoryEntry* lastChildFile = nullptr;
        int canRead;
        int canWrite;
    };

public:
    DirectoryManage();
    ~DirectoryManage();

    void login();
    User QueryUser(std::string account, std::string password);
    void CreateRootDirectory();
    void tree(Directory* curDir, int level);
    void MakeDir(std::string name);
    void cdDir(std::string path);
    void rd(std::string name);
    void DeleteDir(Directory* p);
    void ListDir(Directory* p);
    void ListAll();
    void ListByName(const std::string& name);
    void dirFile(std::string name);
    void dirPath(std::string path);
    void dir();
    void ren(std::string name, std::string newName);
    void RenameDir(Directory* ptr, std::string newName);
    void RenameFile(DirectoryEntry* ptr, std::string newName);
    std::string GetPath(Directory* curDir);
    File CreateFile(std::string name);
    void CreateDirEntry(File file, int first_block);
    bool DeleteFile(std::string name);
    File* ReadFile(std::string path);
    File* WriteFile(std::string name);
    File* OpenFile(std::string name);
    File* CloseFile(std::string name);

    Directory* getWorkDir() const { return workDir_; }
    Directory* getRootDir() const { return rootDir_; }
    std::string getUserName() const { return user_.name; }

private:
    Directory* workDir_ = nullptr;
    Directory* rootDir_ = nullptr;
    std::string path_;
    User user_;
};
