#include "cli.h"

CLI::CLI(ThreadManager& tm, DirectoryManage& dir, DiskManager& disk, MemoryManager& mem)
    : tm_(tm), dir_(dir), disk_(disk), mem_(mem)
{}

void CLI::showMenu()
{
    dir_.login();
    std::string sel;
    std::cout << "请输入你要实现的功能(输入命令）：\n"
              << "mkdir rmdir cd ls rm touch tree readfile readDisk write rename refresh quit\n"
              << "disk(查看磁盘信息)" << std::endl;

    int main_status = 0;
    while (!main_status)
    {
        input_mutex_.lock();
        refresh();
        std::cin >> sel;
        input_mutex_.unlock();
        auto it = branch_table_.find(sel);
        if (it == branch_table_.end())
        {
            it = branch_table_.find("default");
        }
        switch (it->second)
        {
        case 1:
        {
            std::string name;
            std::cout << "请输入目录名：";
            std::cin >> name;
            dir_.MakeDir(name);
            break;
        }
        case 2:
        {
            std::string name;
            std::cout << "请输入目录名：";
            std::cin >> name;
            dir_.rd(name);
            break;
        }
        case 3:
        {
            std::string name;
            std::cout << "请输入相对路径：";
            std::cin >> name;
            dir_.cdDir(name);
            break;
        }
        case 4:
        {
            dir_.dir();
            break;
        }
        case 5:
        {
            std::string name;
            std::cout << "请输入文件名：";
            std::cin >> name;
            std::cout << "后台处理中，您可继续执行其他命令..." << std::endl;
            tm_.spawnThread(std::thread(&ThreadManager::delete_data_thread, &tm_, name));
            break;
        }
        case 6:
        {
            std::string name;
            std::cout << "请输入文件名：";
            std::cin >> name;
            std::cout << "后台处理中，您可继续执行其他命令..." << std::endl;
            tm_.spawnThread(std::thread(&ThreadManager::data_generation_thread, &tm_, name));
            break;
        }
        case 7:
        {
            dir_.tree(dir_.getWorkDir(), -1);
            break;
        }
        case 8:
        {
            std::string name;
            std::cout << "请输入当前目录下文件名：";
            std::cin >> name;
            std::cout << "执行线程：处理中..." << std::endl;
            tm_.spawnThread(std::thread(&ThreadManager::execute_thread, &tm_, name));
            break;
        }
        case 9:
        {
            std::string name;
            std::cout << "请输入文件名：";
            std::cin >> name;
            dir_.WriteFile(name);
            break;
        }
        case 10:
        {
            std::string oldName, newName;
            std::cout << "请输入旧文件名和新文件名，中间以空格间隔：";
            std::cin >> oldName >> newName;
            dir_.ren(oldName, newName);
            break;
        }
        case 11:
            std::cout << "退出系统。\n";
            main_status = 1;
            break;
        case 13:
            refresh();
            break;
        case 14:
        {
            std::cout << "空闲块信息（成组链接法超级块）：" << std::endl;
            disk_.printFreeBlocks();
            break;
        }
        case 15:
        {
            std::cout << "请输入要读取的磁盘块号：" << std::endl;
            short num_block;
            std::cin >> num_block;
            std::string data = disk_.readBlock(num_block);
            std::cout << data << std::endl;
            break;
        }
        default:
            std::cout << "该命令不存在，请重新输入" << std::endl;
            break;
        }
        disk_.dumpFile();
    }
    disk_.dumpFile();
}

void CLI::refresh()
{
    std::string path = dir_.GetPath(dir_.getWorkDir());
    std::cout << "主线程：" << path << ">:";
}
