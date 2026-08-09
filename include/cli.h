#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <sstream>
#include <limits>

#include "threadManage.h"

class CLI
{
public:
    CLI(ThreadManager& tm, DirectoryManage& dir, DiskManager& disk, MemoryManager& mem);
    void showMenu();

private:
    struct ShellCmd {
        std::vector<std::string> argv;
        char op = '\0';
        std::string out_file;
        std::string pipe_in_;
    };

    void refresh();
    int dispatch(const std::string& cmd, const std::vector<std::string>& args,
                 const std::string& pipeIn = "");
    std::vector<ShellCmd> parse(const std::string& line);
    ShellCmd make_cmd_(const std::string& line);
    std::vector<std::string> argv_first_tok_(const std::string& s);
    std::vector<std::string> argv_slice_(const std::vector<std::string>& v);
    void execute(std::vector<ShellCmd>& cmds);
    int builtin_test_(const std::vector<std::string>& a);

    void print_name_(const std::string& name);
    void ls_all();
    void ls_long(const std::string& name);
    void du_dir();
    std::string human_size(int bytes);

    // ANSI color helpers
    std::string red(const std::string& s) { return "\033[31m" + s + "\033[0m"; }
    std::string green(const std::string& s) { return "\033[32m" + s + "\033[0m"; }
    std::string yellow(const std::string& s) { return "\033[33m" + s + "\033[0m"; }
    std::string blue(const std::string& s) { return "\033[34m" + s + "\033[0m"; }
    std::string magenta(const std::string& s) { return "\033[35m" + s + "\033[0m"; }
    std::string cyan(const std::string& s) { return "\033[36m" + s + "\033[0m"; }
    std::string white(const std::string& s) { return "\033[37m" + s + "\033[0m"; }
    std::string bold(const std::string& s) { return "\033[1m" + s + "\033[0m"; }

    ThreadManager& tm_;
    DirectoryManage& dir_;
    DiskManager& disk_;
    MemoryManager& mem_;

    std::unordered_map<std::string, std::function<int(const std::vector<std::string>&)>> branch_table_;
    std::unordered_map<std::string, std::string> help_text_;
    std::unordered_map<std::string, std::string> env_;
    std::unordered_map<std::string, std::string> aliases_;
    std::vector<std::string> history_;

    int last_exit_ = 0;
    bool running_ = true;
};