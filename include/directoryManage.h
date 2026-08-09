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
    User QueryUser(const std::string& account, const std::string& password);
    void CreateRootDirectory();
    void tree(Directory* curDir, int level) const;
    void MakeDir(const std::string& name);
    void cdDir(const std::string& path);
    void rd(const std::string& name);
    void DeleteDir(Directory* p);
    std::vector<std::string> listChildNames() const;
    void ListDir(Directory* p) const;
    void ListAll() const;
    void ListByName(const std::string& name) const;
    void dirFile(const std::string& name) const;
    void dirPath(const std::string& path) const;
    void dir();
    void ren(const std::string& name, const std::string& newName);
    void RenameDir(Directory* ptr, const std::string& newName);
    void RenameFile(DirectoryEntry* ptr, const std::string& newName);
    std::string GetPath(Directory* curDir) const;
    File CreateFile(const std::string& name);
    void CreateDirEntry(File file, int first_block);
    bool DeleteFile(const std::string& name);
    File* ReadFile(std::string path) const;
    File* WriteFile(const std::string& name);
    File* OpenFile(std::string name) const;
    File* CloseFile(std::string name) const;

    // Non-mutating accessors for ls/stat formatting
    bool isDir(const std::string& name) const;
    bool isFile(const std::string& name) const;
    const File* getFile(const std::string& name) const;

    Directory* getWorkDir() const { return workDir_; }
    Directory* getRootDir() const { return rootDir_; }
    std::string getUserName() const { return user_.name; }

private:
    Directory* workDir_ = nullptr;
    Directory* rootDir_ = nullptr;
    User user_;
};
