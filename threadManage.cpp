#include "threadManage.h"
void ThreadManager::data_generation_thread(DirectoryManage::File &newFile)
{
    // 调用磁盘分配的函数,若成功返回1，失败返回-1
    // int status = AllocateBlocks(newFile.name, newFile.size, newFile.context);
    // if (status == 1)
    // {
    //     // 成功分配内存块
    //     int first_block = diskManager.fileNameToNumOfBlock[newFile->name]; // 获取文件名对应的第一个盘块号
    // }
    // else
    // {
    // }
}

void ThreadManager::delete_data_thread(string name)
{
    cout << "处理中..." << endl;
    int sleep_time = this->generateNumber(1000, 5000); // 1-5秒的时延，体现多线程机制
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    DIR.DeleteFile(name);
    /*TODO:回收外存块*/
}

int ThreadManager::generateNumber(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

ThreadManager::ThreadManager()
{
    initialize();
}

int ThreadManager::initialize()
{
    return 0;
}

void ThreadManager::showMenu()
{
    DIR.login();
    int sel;
    cout << "请输入你要实现的功能(输入数字，不要输入命令本身！！！)：\n"
         << "1.mkdir 2.rmdir 3.cd 4.dir 5.del 6.create 7.tree 8.readFile 9.writeFile 10.rename 11.exit\n";
    while (true)
    {
        string path = "";
        path = DIR.GetPath(DIR.workDir);
        cout << path << ">:";
        cin >> sel;
        switch (sel)
        {
        case 1:
        {
            string name;
            cout << "请输入目录名：";
            cin >> name;
            DIR.MakeDir(name);
            break;
        }

        case 2:
        {
            string name;
            cout << "请输入目录名：";
            cin >> name;
            DIR.rd(name); // 删除当前目录下的目录
            break;
        }
        case 3:
        {
            string name;
            cout << "请输入相对路径：";
            cin >> name;
            DIR.cdDir(name); // 相对路径
            break;
        }
        case 4:
        {
            DIR.dir();
            break;
        }
        case 5:
        {
            string name;
            cout << "请输入文件名：";
            cin >> name;
            thread t(&ThreadManager::delete_data_thread, this, name);
            break;
        }
        case 6:
        {
            string name;
            cout << "请输入文件名：";
            cin >> name;
            DIR.CreateFile(name);
            break;
        }
        case 7:
        {
            DIR.tree(DIR.workDir, -1);
            break;
        }
        case 8:
        {
            string path;
            cout << "请输入文件名所在路径：";
            cin >> path;
            DIR.ReadFile(path);
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
        case 11:
            exit(0);
        default:
            exit(0);
        }
    }
}
