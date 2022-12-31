#include "threadManage.h"
void ThreadManager::data_generation_thread(string fileName)
{

    input_mutex.lock();
    DirectoryManage::File *newFile = DIR.CreateFile(fileName);
    input_mutex.unlock();
    cout << "后台处理中，您可继续执行其他命令..." << endl;
    // 调用磁盘分配的函数,若成功返回1，失败返回-1
    int status = DIM.AllocateBlocks(newFile->name, newFile->size / BLOCK_SIZE + 1, newFile->context); // file->size 表示字符数，需要除以BLOCK_SIZE(40)+1才是所需盘块
    if (status == 1)
    {
        int sleep_time = this->generateNumber(2000, 5000); // 2-5秒的时延，体现多线程机制
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        // 成功分配内存块
        int first_block = DIM.fileNameToNumOfBlock[newFile->name]; // 获取文件名对应的第一个盘块号
        cout << "数据生成进程：文件磁盘块分配成功，起始的磁盘号为：" << first_block << endl;
        cout << "数据生成进程：创建目录项中..." << endl;
        sleep_time += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        DIR.CreateDirEntry(*newFile, first_block);
        cout << "数据生成进程：目录项创建成功！" << endl;
    }
    else
    {
        cout << "数据生成进程：磁盘空间不足，磁盘块分配失败！" << endl;
    }
}

void ThreadManager::delete_data_thread(string name)
{
    int sleep_time = this->generateNumber(2000, 5000); // 2-5秒的时延，体现多线程机制
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    bool isDelFormDir = DIR.DeleteFile(name); // 目录中没有这个文件返回false，有这个文件返回并删除返回true
    if (isDelFormDir)
    {
        cout << "数据删除进程：该文件在目录中删除，正在回收外存..." << endl;
        sleep_time += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        cout << "数据删除进程：外存回收成功！" << endl;
        // TODO：展示哪些磁盘块被释放
        // cout << "被回收的磁盘块号为：";
        // for (auto i : DIM.getAllBlocks(name))
        // {
        //     cout << i << " ";
        // }
        // cout << endl;
        DIM.DeallocateBlocks(name); /*回收外存块*/
    }
    else
    {
        cout << "数据删除进程：不存在该文件！" << endl;
    }
}

void ThreadManager::execute_thread(string name)
{
    DirectoryManage::File *newFile = DIR.ReadFile(name);
    if (newFile) // 文件存在，且有权限
    {
        // 调用内存管理的空闲空间管理功能，申请8块内存空间
    }
    else
    {
        cout << "执行进程：文件不存在或权限不足！"
    }
}

void ThreadManager::joinAllThreads()
{
    for (auto &t : threads)
    {
        t.join();
    }
}

int ThreadManager::generateNumber(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

// void ThreadManager::myCout(string output)
// {
//     cout << output << endl;
//     refresh();
// }

ThreadManager::ThreadManager()
{
    // 初始化外存，目录，内存等
    initialize();
}

int ThreadManager::initialize()
{
    return 0;
}

void ThreadManager::refresh()
{
    string path = "";
    path = DIR.GetPath(DIR.workDir);
    cout << "主线程：" << path << ">:";
}

void ThreadManager::showMenu()
{
    DIR.login();
    string sel;
    cout << "请输入你要实现的功能(输入命令）：\n"
         << "mkdir rmdir cd ls rm touch tree read write rename refresh quit\n"
         << "disk(查看磁盘信息)" << endl;
    while (true)
    {
        input_mutex.lock();
        refresh();
        cin >> sel;
        input_mutex.unlock();
        auto it = this->branch_table.find(sel); // 使用unordered_map的find函数来查找对应的整数值
        if (it == branch_table.end())           // 如果没找到，则使用默认值
        {
            it = branch_table.find("default");
        }
        switch (it->second)
        {
        case 1: // mkdir
        {
            string name;
            cout << "请输入目录名：";
            cin >> name;
            DIR.MakeDir(name);
            break;
        }

        case 2: // rmdir
        {
            string name;
            cout << "请输入目录名：";
            cin >> name;
            DIR.rd(name); // 删除当前目录下的目录
            break;
        }
        case 3: // cd
        {
            string name;
            cout << "请输入相对路径：";
            cin >> name;
            DIR.cdDir(name); // 相对路径
            break;
        }
        case 4: // ls
        {
            DIR.dir();
            break;
        }
        case 5: // rm
        {
            string name;
            string content;
            cout << "请输入文件名：";
            cin >> name;
            // thread t(&ThreadManager::delete_data_thread, this, name);
            cout << "后台处理中，您可继续执行其他命令..." << endl;
            threads.emplace_back(&ThreadManager::delete_data_thread, this, name); // 创建一个线程，并调用delete_data_thread（）
            break;
        }
        case 6: // touch
        {
            string name;
            cout << "请输入文件名：";
            cin >> name;
            threads.emplace_back(&ThreadManager::data_generation_thread, this, name);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            break;
        }
        case 7: // tree
        {
            DIR.tree(DIR.workDir, -1);
            break;
        }
        case 8: // readFile
        {
            string name;
            cout << "请输入当前目录下文件名：";
            cin >> name;
            cout << "执行线程：处理中..." << endl;
            threads.emplace_back(&ThreadManager::execute_thread, this, name);
            // DIR.ReadFile(path);
            break;
        }
        case 9:
        {
            string name;
            cout << "请输入文件名：";
            cin >> name;
            DIR.WriteFile(name);
            break;
        }
        case 10:
        {
            string oldName, newName;
            cout << "请输入旧文件名和新文件名，中间以空格间隔：";
            cin >> oldName;
            cin >> newName;
            DIR.ren(oldName, newName);
            break;
        }
        case 11: // quit
            exit(0);
        case 13:
            break;
        default:
        {
            cout << "该命令不存在，请重新输入" << endl;
        }
        }
    }
}
