#include "DirectoryManage.h"
#include "threadManage.h"
// 创建根目录
void DirectoryManage::CreateRootDirectory()
{
    rootDir->name = "root";
    rootDir->firstChildDirectory = NULL;
    rootDir->firstChildFile = NULL;
    rootDir->lastChildFile = NULL;
    rootDir->frontDirectory = NULL;
    rootDir->nextDirectory = NULL;
    rootDir->parentDirectory = NULL;
    rootDir->canRead = 1;  // 根目录可读
    rootDir->canWrite = 0; // 根目录不可写
    cout << "创建根目录成功！\n";
}
// 新建目录：新建一个空目录
void DirectoryManage::MakeDir(string name)
{
    Directory *newDir = new Directory();
    newDir->name = name;
    newDir->firstChildDirectory = NULL;
    newDir->firstChildFile = NULL;
    newDir->lastChildFile = NULL;
    newDir->frontDirectory = NULL;
    newDir->nextDirectory = NULL;
    newDir->parentDirectory = NULL;
    newDir->canRead = user.userRight;
    newDir->canWrite = user.userRight;

    if (workDir->firstChildDirectory == NULL)
    {
        workDir->firstChildDirectory = newDir;
    }
    else
    {
        Directory *ptr = workDir->firstChildDirectory;
        while (true)
        {
            if (ptr->name == name)
            {
                cout << "该目录名称已存在，请修改名称后重新创建目录！\n";
                return;
            }
            if (ptr->nextDirectory)
            {
                ptr = ptr->nextDirectory;
            }
            else
            {
                ptr->nextDirectory = newDir;
                newDir->frontDirectory = ptr;
                break;
            }
        }
    }
    cout << "新目录" << name << "创建成功！\n";
}
// 删除目录：删除目录及目录中的文件
void DirectoryManage::DeleteDir(string name)
{
}
// 显示当前目录下所有文件夹和文件
void DirectoryManage::ListAll()
{
    queue<Directory *> q;
    q.push(rootDir);
    while (!q.empty())
    {
        Directory *p = q.front();
        q.pop();
        if (p->firstChildDirectory || p->firstChildFile)
        {
            GetPath(p);
            cout << path << "下子目录及文件\n";
        }
        if (p->firstChildDirectory)
        {
            p = p->firstChildDirectory;
            cout << '\t' << p->name << '\t';
            cout << "<DIR>\n";
            q.push(p);
            while (p->nextDirectory)
            {
                p = p->nextDirectory;
                cout << '\t' << p->name << '\t';
                cout << "<DIR>\n";
                q.push(p);
            }
        }
        if (p->firstChildFile)
        {
            DirectoryEntry *ptr = p->firstChildFile;
            cout << '\t' << ptr->file.name << '\t';
            cout << "<FILE>\n";
            while (ptr->nextFile)
            {
                ptr = ptr->nextFile;
                cout << '\t' << ptr->file.name << '\t';
                cout << "<FILE>\n";
            }
        }
    }
}
// 修改目录：重命名
void DirectoryManage::ModifyDir(string name, string newName)
{
    if (name == newName)
        return;
    queue<Directory *> q;
    q.push(rootDir);
    while (!q.empty())
    {
        Directory *ptr = q.front();
        q.pop();
        if (ptr->name == name && ptr->canWrite != 1 && ptr->canWrite != user.userRight)
        { // 权限判断
            cout << "用户权限不足，无法修改！\n";
            return;
        }
        else if (ptr->name == name)
        {
            // 重名判断
            Directory *p = ptr->parentDirectory->firstChildDirectory;
            while (p)
            {
                if (p->name == newName)
                {
                    cout << "该目录名称已存在，无法重命名该目录！\n";
                    return;
                }
                p = p->nextDirectory;
            }
            ptr->name = newName;
            return;
        }
        if (ptr->firstChildDirectory)
        {
            ptr = ptr->firstChildDirectory;
            q.push(ptr);
            while (ptr->nextDirectory)
            {
                ptr = ptr->nextDirectory;
                q.push(ptr);
            }
        }
    }
}
// 获取当前文件夹的路径
void DirectoryManage::GetPath(Directory *curDir)
{
    path = "";
    stack<Directory *> q;
    q.push(curDir);
    while (curDir->parentDirectory)
    {
        q.push(curDir->parentDirectory);
        curDir = curDir->parentDirectory;
    }
    while (!q.empty())
    {
        Directory *p = q.top();
        q.pop();
        path += p->name;
        if (!q.empty())
            path += "\\";
    }
}
// 创建文件：在此方法中输入数据大小、数据信息、存储目录、文件名等，然后调用数据生成线程来创建文件。
void DirectoryManage::CreateFile(string name)
{
    // 检查文件是否重名
    DirectoryEntry *p = workDir->firstChildFile;
    while (p)
    {
        if (p->file.name == name)
        {
            cout << "该文件名称已存在，请更改文件名后重新创建文件！\n";
            return;
        }
        p = p->nextFile;
    }
    File *newFile = new File();
    newFile->name = name;
    newFile->size = newFile->context.length();
    getline(cin, newFile->context);
    time_t create_time;
    time(&create_time);
    newFile->structure = "text";
    newFile->canRead = user.userRight;
    newFile->canWrite = user.userRight;
    // 执行数据生成线程
    // 线程：调用磁盘分配的函数，调用新建文件目录项的函数
    thread t(data_generation_thread, std::ref(newFile)); // 运行到这边就开一个新的线程执行data_generation_thread函数
    t.join();
}
// 为文件建立目录项
void DirectoryManage::CreateDirEntry(File file, string owner, time_t create_time, int first_block)
{
    DirectoryEntry *newDirEntry = new DirectoryEntry();
    newDirEntry->file = file;
    newDirEntry->owner = owner; // 改成 newDirEntry->owner = user.name
    newDirEntry->create_time = create_time;
    newDirEntry->first_block = first_block;
    newDirEntry->frontFile = workDir->lastChildFile;
    workDir->lastChildFile->nextFile = newDirEntry;
    workDir->lastChildFile = newDirEntry;
    newDirEntry->nextFile = NULL;
    newDirEntry->parentDirectory = workDir;
}
// 删除文件：删除文件的目录项
void DirectoryManage::DeleteFile(string name)
{
}
// 读文件：可读文件时才能读
void DirectoryManage::ReadFile(string name)
{
}
// 写文件：可写文件时才能写
void DirectoryManage::WriteFile(string name)
{
}
// 修改文件：重命名或修改文件权限
void DirectoryManage::ModifyFile(string name)
{
    string newName;
    cin >> newName;
    if (name == newName)
        return;
    queue<Directory *> q;
    q.push(rootDir);
    while (!q.empty())
    {
        Directory *ptr = q.front();
        q.pop();
        if (ptr->name == name && ptr->canWrite != 1 && ptr->canWrite != user.userRight)
        { // 权限判断
            cout << "用户权限不足，无法修改！\n";
            return;
        }
        else if (ptr->name == name)
        {
            // 重名判断
            Directory *p = ptr->parentDirectory->firstChildDirectory;
            while (p)
            {
                if (p->name == newName)
                {
                    cout << "该文件名称已存在，无法重命名该文件！\n";
                    return;
                }
                p = p->nextDirectory;
            }
            ptr->name = newName;
            return;
        }
        if (ptr->firstChildDirectory)
        {
            ptr = ptr->firstChildDirectory;
            q.push(ptr);
            while (ptr->nextDirectory)
            {
                ptr = ptr->nextDirectory;
                q.push(ptr);
            }
        }
    }
}
// 文件的打开
void DirectoryManage::OpenFile(string name)
{
    // 执行线程，将文件调入内存
    //  执行线程：调用分配内存块函数
    // 让用户选择读文件还是写文件
}
// 文件的关闭
void DirectoryManage::CloseFile(string name)
{
}
