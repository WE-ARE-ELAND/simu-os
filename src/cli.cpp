#include "cli.h"

CLI::CLI(ThreadManager& tm, DirectoryManage& dir, DiskManager& disk, MemoryManager& mem)
    : tm_(tm), dir_(dir), disk_(disk), mem_(mem)
{}

void CLI::showMenu()
{
    dir_.login();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Available commands: mkdir rmdir cd ls rm touch tree readfile readDisk write rename refresh disk quit\n";

    branch_table_ = {
        {"mkdir", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: mkdir <dirname>\n"; return; }
            dir_.MakeDir(a[0]);
        }},
        {"rmdir", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: rmdir <dirname>\n"; return; }
            dir_.rd(a[0]);
        }},
        {"cd", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: cd <path>\n"; return; }
            dir_.cdDir(a[0]);
        }},
        {"ls", [this](const std::vector<std::string>& a) {
            if (a.empty()) {
                dir_.ListAll();
            } else if (a.size() == 2 && a[0] == "-l") {
                dir_.dirFile(a[1]);
            } else if (a.size() == 2 && a[0] == "-d") {
                dir_.dirPath(a[1]);
            } else if (a.size() == 1) {
                dir_.ListByName(a[0]);
            } else {
                std::cout << "Usage: ls [-l <file>] [-d <dir>] [<name>]\n";
            }
        }},
        {"rm", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: rm <filename>\n"; return; }
            std::cout << "Deleting in background...\n";
            tm_.spawnThread(std::thread(&ThreadManager::delete_data_thread, &tm_, a[0]));
        }},
        {"touch", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: touch <filename>\n"; return; }
            std::cout << "Creating in background...\n";
            tm_.spawnThread(std::thread(&ThreadManager::data_generation_thread, &tm_, a[0]));
        }},
        {"tree", [this](const std::vector<std::string>&) {
            dir_.tree(dir_.getWorkDir(), -1);
        }},
        {"readfile", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: readfile <filename>\n"; return; }
            std::cout << "Executing: loading file...\n";
            tm_.spawnThread(std::thread(&ThreadManager::execute_thread, &tm_, a[0]));
        }},
        {"write", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: write <filename>\n"; return; }
            dir_.WriteFile(a[0]);
        }},
        {"rename", [this](const std::vector<std::string>& a) {
            if (a.size() < 2) { std::cout << "Usage: rename <oldname> <newname>\n"; return; }
            dir_.ren(a[0], a[1]);
        }},
        {"quit", [this](const std::vector<std::string>&) {
            std::cout << "Exiting.\n";
            running_ = false;
        }},
        {"refresh", [this](const std::vector<std::string>&) {
            refresh();
        }},
        {"disk", [this](const std::vector<std::string>&) {
            std::cout << "Free block information (grouped free list):\n";
            disk_.printFreeBlocks();
        }},
        {"readDisk", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: readDisk <block_number>\n"; return; }
            try {
                short num_block = static_cast<short>(std::stoi(a[0]));
                std::string data = disk_.readBlock(num_block);
                std::cout << data << "\n";
            } catch (const std::exception& e) {
                std::cout << "Invalid block number: " << a[0] << "\n";
            }
        }}
    };

    std::string line;
    while (running_)
    {
        input_mutex_.lock();
        refresh();
        std::getline(std::cin, line);
        input_mutex_.unlock();

        std::vector<std::string> tokens;
        std::istringstream iss(line);
        std::string token;
        while (iss >> token)
        {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        std::string cmd = tokens[0];
        tokens.erase(tokens.begin());

        auto it = branch_table_.find(cmd);
        if (it != branch_table_.end())
        {
            it->second(tokens);
        }
        else
        {
            std::cout << "Unknown command: " << cmd << "\n";
        }
        disk_.dumpFile();
    }
    disk_.dumpFile();
}

void CLI::refresh()
{
    std::string path = dir_.GetPath(dir_.getWorkDir());
    std::cout << "[" << dir_.getUserName() << "@simuos " << path << "]$ ";
}