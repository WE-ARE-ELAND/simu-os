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

    // ANSI color helpers (auto-disable when stdout is not a terminal)
    std::string _col_(const std::string& code, const std::string& s) {
        return enable_color_ ? ("\033[" + code + "m" + s + "\033[0m") : s;
    }
    std::string red(const std::string& s) { return _col_("31", s); }
    std::string green(const std::string& s) { return _col_("32", s); }
    std::string yellow(const std::string& s) { return _col_("33", s); }
    std::string blue(const std::string& s) { return _col_("34", s); }
    std::string magenta(const std::string& s) { return _col_("35", s); }
    std::string cyan(const std::string& s) { return _col_("36", s); }
    std::string white(const std::string& s) { return _col_("37", s); }
    std::string bold(const std::string& s) { return _col_("1", s); }

    ThreadManager& tm_;
    DirectoryManage& dir_;
    DiskManager& disk_;
    MemoryManager& mem_;

    std::unordered_map<std::string, std::function<int(const std::vector<std::string>&)>> branch_table_;
    std::unordered_map<std::string, std::string> help_text_;
    std::unordered_map<std::string, std::string> env_;
    std::unordered_map<std::string, std::string> aliases_;
    std::vector<std::string> history_;
    bool enable_color_ = true;

    int last_exit_ = 0;
    bool running_ = true;
};