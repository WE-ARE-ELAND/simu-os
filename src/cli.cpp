#include "cli.h"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif

// ===========================================================================
// Helpers
// ===========================================================================

static void cli_enableVT()
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

static bool cli_isatty()
{
#ifdef _WIN32
    return _isatty(_fileno(stdin)) != 0;
#else
    return isatty(STDIN_FILENO) != 0;
#endif
}

static bool cli_glob_match(const std::string& name, size_t si,
                           const std::string& pat, size_t pi)
{
    while (pi < pat.size() && si < name.size())
    {
        if (pat[pi] == '*')
        {
            ++pi;
            if (pi == pat.size()) return true;
            while (si < name.size())
            {
                if (cli_glob_match(name, si, pat, pi)) return true;
                ++si;
            }
            return false;
        }
        if (pat[pi] == '?' || pat[pi] == name[si]) { ++pi; ++si; }
        else return false;
    }
    return pi == pat.size() && si == name.size();
}

static std::vector<std::string> cli_tokenize(
        const std::string& line,
        const std::unordered_map<std::string, std::string>& env,
        int lastExit,
        const std::vector<std::string>& names)
{
    std::vector<std::string> tokens;
    std::string cur;
    bool in_sq = false, in_dq = false, esc = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (esc) { cur += c; esc = false; continue; }
        if (c == '\\') { esc = true; continue; }
        if (c == '\'' && !in_dq) { in_sq = !in_sq; continue; }
        if (c == '"'  && !in_sq) { in_dq = !in_dq; continue; }

        if (c == '$' && !in_sq)
        {
            if (i + 1 < line.size() && line[i + 1] == '{')
            {
                size_t end = line.find('}', i + 2);
                if (end != std::string::npos)
                {
                    std::string key = line.substr(i + 2, end - i - 2);
                    auto it = env.find(key);
                    cur += (it != env.end()) ? it->second : "";
                    i = end;
                    continue;
                }
            }
            if (i + 1 < line.size() && line[i + 1] == '?')
            { cur += std::to_string(lastExit); ++i; continue; }
            std::string key;
            while (i + 1 < line.size() &&
                   (std::isalnum(static_cast<unsigned char>(line[i + 1])) || line[i + 1] == '_'))
            { ++i; key += line[i]; }
            auto it = env.find(key);
            cur += (it != env.end()) ? it->second : "";
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)) && !in_sq && !in_dq)
        {
            if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
            continue;
        }
        cur += c;
    }
    if (!cur.empty()) tokens.push_back(cur);

    // Glob expansion
    for (size_t ti = 0; ti < tokens.size(); ++ti)
    {
        auto& tok = tokens[ti];
        if (tok.find('*') == std::string::npos && tok.find('?') == std::string::npos)
            continue;
        std::vector<std::string> matches;
        for (const auto& n : names)
            if (cli_glob_match(n, 0, tok, 0)) matches.push_back(n);
        if (!matches.empty()) tok = matches[0];
    }
    return tokens;
}

// ===========================================================================
// Raw-input readline
// ===========================================================================

static std::string cli_readLine(std::vector<std::string>& history)
{
    if (!cli_isatty())
    {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    std::string buf;
    size_t cursor = 0;
    size_t hist_pos = history.size();

    auto redraw = [&]()
    {
        std::cout << "\r\033[K" << buf;
        if (cursor < buf.size())
        {
            unsigned n = static_cast<unsigned>(buf.size() - cursor);
            std::cout << "\r\033[K" << buf.substr(0, cursor) << "\033[" << n << "D";
        }
        std::cout.flush();
    };

    while (true)
    {
        char c = static_cast<char>(std::cin.get());
        if (std::cin.eof()) { std::cout << "\n"; return "__EOF__"; }
        if (c == '\r' || c == '\n') { std::cout << "\n"; break; }
        if (c == 3)              // Ctrl-C
        { buf.clear(); cursor = 0; redraw(); continue; }
        if (c == 4)              // Ctrl-D
        { if (buf.empty()) { std::cout << "\n"; return "__EOF__"; }
          if (cursor > 0) { buf.erase(cursor - 1, 1); --cursor; redraw(); }
          continue; }
        if (c == '\t') continue;             // placeholder
        if (c == 27)
        {
            char s1 = static_cast<char>(std::cin.get());
            if (s1 != '[') continue;
            char s2 = static_cast<char>(std::cin.get());
            if (s2 == 'A')
            {
                if (hist_pos > 0)
                {
                    if (hist_pos == history.size()) history.push_back(buf);
                    --hist_pos;
                    buf = history[hist_pos];
                    cursor = buf.size();
                    redraw();
                }
            }
            else if (s2 == 'B')
            {
                if (hist_pos < history.size() - 1)
                { ++hist_pos; buf = history[hist_pos]; cursor = buf.size(); redraw(); }
                else if (hist_pos == history.size() - 1)
                { ++hist_pos; buf.clear(); cursor = 0; redraw(); }
            }
            else if (s2 == 'C') { if (cursor < buf.size()) { ++cursor; redraw(); } }
            else if (s2 == 'D') { if (cursor > 0) { --cursor; redraw(); } }
            else if (s2 == 'H') { cursor = 0; redraw(); }
            else if (s2 == 'F') { cursor = buf.size(); redraw(); }
            continue;
        }
        if (c >= 32 && c < 127)
        { buf.insert(static_cast<long>(cursor), 1, c); ++cursor; redraw(); }
    }
    return buf;
}

// ===========================================================================
// CLI constructor
// ===========================================================================

CLI::CLI(ThreadManager& tm, DirectoryManage& dir, DiskManager& disk, MemoryManager& mem)
    : tm_(tm), dir_(dir), disk_(disk), mem_(mem)
{
    cli_enableVT();
    env_["HOME"]  = "/";
    env_["USER"]  = "";
    env_["SHELL"] = "simu-sh";
    env_["PWD"]   = "/";
    env_["TERM"]  = "xterm-256color";
}

// ===========================================================================
// showMenu
// ===========================================================================

void CLI::showMenu()
{
    dir_.login();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    env_["USER"] = dir_.getUserName();

    help_text_ = {
        {"mkdir",    "mkdir <dir...>         Create directories"},
        {"rmdir",    "rmdir <dir...>         Remove empty directories"},
        {"cd",       "cd <path>              Change directory"},
        {"ls",       "ls [-l] [-a] [<name>...]  List directory contents"},
        {"rm",       "rm <file...>           Delete files"},
        {"touch",    "touch <file>           Create a new file"},
        {"tree",     "tree                   Show directory tree"},
        {"readfile", "readfile <file>        Read file into memory"},
        {"readDisk", "readDisk <block>       Read a raw disk block"},
        {"write",    "write <file>           Append content to a file"},
        {"rename",   "rename <old> <new>     Rename a file or directory"},
        {"quit",     "quit                   Exit the system"},
        {"disk",     "disk                   Show free-block information"},
        {"refresh",  "refresh                Redraw the prompt"},
        {"pwd",      "pwd                    Print working directory"},
        {"whoami",   "whoami                 Print current user"},
        {"date",     "date                   Print current date/time"},
        {"echo",     "echo [-n] [arg...]     Print arguments"},
        {"clear",    "clear                  Clear the screen"},
        {"history",  "history                Show command history"},
        {"alias",    "alias [name=cmd]       Define or list aliases"},
        {"unalias",  "unalias <name>         Remove an alias"},
        {"export",   "export KEY=VALUE       Set environment variable"},
        {"env",      "env                    List environment variables"},
        {"type",     "type <name>            Show command type"},
        {"test",     "test <expr>            Evaluate a condition"},
        {"help",     "help [cmd]             Show this help"},
        {"du",       "du                     Show disk usage in current dir"},
        {"df",       "df                     Show disk space usage"},
        {"free",     "free                   Show memory usage"},
        {"who",      "who                    Show logged-in user"},
        {"stat",     "stat <file>            Show detailed file info"},
    };

    branch_table_ = {
        {"mkdir", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: mkdir <dirname>\n"; return 1; }
            for (auto& x : a) { dir_.MakeDir(x); }
            return 0; }},
        {"rmdir", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: rmdir <dirname>\n"; return 1; }
            for (auto& x : a) { dir_.rd(x); }
            return 0; }},
        {"cd", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: cd <path>\n"; return 1; }
            dir_.cdDir(a[0]);
            env_["PWD"] = dir_.GetPath(dir_.getWorkDir());
            return 0; }},
        {"ls", [this](const std::vector<std::string>& a) {
            bool longFmt = false;
            std::vector<std::string> names;
            for (auto& x : a)
            {
                if (x == "-l" || x == "-al" || x == "-la") longFmt = true;
                else if (x == "-a" || x == "-la" || x == "-al") { /* no-op */ }
                else names.push_back(x);
            }
            if (names.empty()) { longFmt ? ls_long("__ALL__") : ls_all(); }
            else { for (auto& n : names) { longFmt ? ls_long(n) : print_name_(n); } }
            return 0; }},
        {"rm", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: rm <filename>\n"; return 1; }
            std::cout << "Deleting in background...\n";
            tm_.spawnThread(std::thread(&ThreadManager::delete_data_thread, &tm_, a[0]));
            return 0; }},
        {"touch", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: touch <filename>\n"; return 1; }
            std::cout << "Creating in background...\n";
            tm_.spawnThread(std::thread(&ThreadManager::data_generation_thread, &tm_, a[0]));
            return 0; }},
        {"tree", [this](const std::vector<std::string>&) {
            std::cout << cyan(dir_.GetPath(dir_.getWorkDir())) << "\n";
            dir_.tree(dir_.getWorkDir(), -1); return 0; }},
        {"readfile", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: readfile <filename>\n"; return 1; }
            std::cout << "Executing: loading file...\n";
            tm_.spawnThread(std::thread(&ThreadManager::execute_thread, &tm_, a[0]));
            return 0; }},
        {"write", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: write <filename>\n"; return 1; }
            dir_.WriteFile(a[0]); return 0; }},
        {"rename", [this](const std::vector<std::string>& a) {
            if (a.size() < 2) { std::cout << "Usage: rename <oldname> <newname>\n"; return 1; }
            dir_.ren(a[0], a[1]); return 0; }},
        {"quit", [this](const std::vector<std::string>&) {
            std::cout << "Exiting.\n"; running_ = false; return 0; }},
        {"refresh", [this](const std::vector<std::string>&) { refresh(); return 0; }},
        {"disk", [this](const std::vector<std::string>&) {
            std::cout << "Free block information (grouped free list):\n";
            disk_.printFreeBlocks(); return 0; }},
        {"readDisk", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: readDisk <block_number>\n"; return 1; }
            short num_block;
            try { num_block = static_cast<short>(std::stoi(a[0])); }
            catch (const std::exception&) { std::cout << "Invalid block number.\n"; return 1; }
            std::cout << disk_.readBlock(num_block) << "\n"; return 0; }},
        {"pwd", [this](const std::vector<std::string>&) {
            std::cout << dir_.GetPath(dir_.getWorkDir()) << "\n"; return 0; }},
        {"whoami", [this](const std::vector<std::string>&) {
            std::cout << dir_.getUserName() << "\n"; return 0; }},
        {"date", [this](const std::vector<std::string>&) {
            std::time_t t = std::time(nullptr);
            std::cout << std::asctime(std::localtime(&t)); return 0; }},
        {"echo", [this](const std::vector<std::string>& a) {
            bool nl = true;
            for (auto& x : a)
            { if (x == "-n") { nl = false; continue; } std::cout << x << " "; }
            if (nl) std::cout << "\n";
            return 0; }},
        {"clear", [this](const std::vector<std::string>&) {
#ifdef _WIN32
            std::system("cls");
#else
            std::system("clear");
#endif
            return 0; }},
        {"history", [this](const std::vector<std::string>&) {
            for (size_t i = 0; i < history_.size(); ++i)
                std::cout << std::setw(4) << (i + 1) << "  " << history_[i] << "\n";
            return 0; }},
        {"alias", [this](const std::vector<std::string>& a) {
            if (a.empty()) { for (auto& kv : aliases_) std::cout << kv.first << "=" << kv.second << "\n"; return 0; }
            for (auto& kv : a)
            {
                auto pos = kv.find('=');
                if (pos == std::string::npos) { std::cout << "Invalid alias: " << kv << "\n"; continue; }
                aliases_[kv.substr(0, pos)] = kv.substr(pos + 1);
            }
            return 0; }},
        {"unalias", [this](const std::vector<std::string>& a) {
            for (auto& n : a) { aliases_.erase(n); }
            return 0; }},
        {"export", [this](const std::vector<std::string>& a) {
            if (a.empty())
            {
                for (auto& kv : env_) { std::cout << kv.first << "=" << kv.second << "\n"; }
                return 0;
            }
            for (auto& kv : a)
            {
                auto pos = kv.find('=');
                if (pos == std::string::npos) { std::cout << "Invalid: " << kv << "\n"; continue; }
                env_[kv.substr(0, pos)] = kv.substr(pos + 1);
            }
            return 0; }},
        {"env", [this](const std::vector<std::string>&) {
            for (auto& kv : env_) { std::cout << kv.first << "=" << kv.second << "\n"; }
            return 0; }},
        {"type", [this](const std::vector<std::string>& a) {
            if (a.empty()) return 1;
            if (aliases_.count(a[0]))     std::cout << a[0] << " is an alias\n";
            else if (branch_table_.count(a[0])) std::cout << a[0] << " is a shell builtin\n";
            else if (dir_.isFile(a[0]))   std::cout << a[0] << " is a file\n";
            else                          std::cout << a[0] << ": not found\n";
            return 0; }},
        {"test", [this](const std::vector<std::string>& a) { return builtin_test_(a); }},
        {"[", [this](std::vector<std::string> a) {
            if (!a.empty() && a.back() == "]") a.pop_back();
            return builtin_test_(a); }},
        {"help", [this](const std::vector<std::string>& a) {
            if (a.empty()) { for (auto& kv : help_text_) std::cout << kv.second << "\n"; return 0; }
            auto it = help_text_.find(a[0]);
            if (it != help_text_.end()) std::cout << it->second << "\n";
            else std::cout << "No help for " << a[0] << "\n";
            return 0; }},
        {"du", [this](const std::vector<std::string>&) { du_dir(); return 0; }},
        {"df", [this](const std::vector<std::string>&) {
            const int FILE_BLOCKS = 900;
            const int BLOCK_SZ    = DiskManager::BLOCK_SIZE;
            int total = FILE_BLOCKS * BLOCK_SZ;
            int free_ = std::min(disk_.getFreeDataBlockNum(), FILE_BLOCKS) * BLOCK_SZ;
            int used  = std::max(0, total - free_);
            std::cout << "Filesystem     " << std::setw(8) << "Size  "
                      << std::setw(8) << "Used  "
                      << std::setw(8) << "Avail "
                      << "Use%  Mounted\n";
            std::cout << "simu-disk      "
                      << std::setw(8) << human_size(total) << "  "
                      << std::setw(8) << human_size(used) << "  "
                      << std::setw(8) << human_size(free_) << "  "
                      << std::setw(4) << (total ? used * 100 / total : 0) << "%   /\n";
            return 0; }},
        {"free", [this](const std::vector<std::string>&) {
            const int MEM_BLOCK = 40;
            const int NUM_MEM   = 64;
            const int TOTAL_SWAP_BLOCKS = 124;
            int totalMem = NUM_MEM * MEM_BLOCK;
            int freeMem  = static_cast<int>(mem_.getFreeBlocks().size()) * MEM_BLOCK;
            int usedMem  = totalMem - freeMem;
            int totalSwap = TOTAL_SWAP_BLOCKS * DiskManager::BLOCK_SIZE;
            int freeSwap  = disk_.getFreeSwapBlockNum() * DiskManager::BLOCK_SIZE;
            int usedSwap  = totalSwap - freeSwap;
            std::cout << "             " << std::setw(7) << "total   "
                      << std::setw(7) << "used    "
                      << std::setw(7) << "free\n";
            std::cout << "Mem:        "
                      << std::setw(7) << human_size(totalMem) << "  "
                      << std::setw(7) << human_size(usedMem) << "  "
                      << std::setw(7) << human_size(freeMem) << "\n";
            std::cout << "Swap:       "
                      << std::setw(7) << human_size(totalSwap) << "  "
                      << std::setw(7) << human_size(usedSwap) << "  "
                      << std::setw(7) << human_size(freeSwap) << "\n";
            return 0; }},
        {"who", [this](const std::vector<std::string>&) {
            std::time_t t = std::time(nullptr);
            std::cout << dir_.getUserName() << "  tty1  "
                      << std::asctime(std::localtime(&t));
            return 0; }},
        {"stat", [this](const std::vector<std::string>& a) {
            if (a.empty()) { std::cout << "Usage: stat <file>\n"; return 1; }
            if (dir_.isDir(a[0]))
            { std::cout << "  File: " << a[0] << "\n  Type: Directory\n"; return 0; }
            auto* f = dir_.getFile(a[0]);
            if (!f) { std::cout << "No such file: " << a[0] << "\n"; return 1; }
            std::string perms;
            for (int i = 0; i < 3; ++i)
            {
                perms += (f->canRead  ? "r" : "-");
                perms += (f->canWrite ? "w" : "-");
            }
            std::cout << "  File: " << f->name << "\n"
                      << "  Type: Regular file\n"
                      << " Size: " << human_size(f->size) << "\n"
                      << "Owner: " << f->owner << "\n"
                      << "Perms: " << perms << "\n"
                      << "Created: " << std::asctime(std::localtime(&f->create_time));
            return 0; }},
    };

    // ==========================================================================
    // Main loop
    // ==========================================================================
    std::string line;
    while (running_)
    {
        refresh();
        line = cli_readLine(history_);
        if (line.empty() || line == "__EOF__") continue;

        history_.push_back(line);
        if (history_.size() > 2000) history_.erase(history_.begin());

        // Strip shell comment (# outside quotes)
        {
            bool sq = false, dq = false, esc = false;
            size_t ci = 0;
            while (ci < line.size())
            {
                char c = line[ci];
                if (esc) { esc = false; ++ci; continue; }
                if (c == '\\') { esc = true; ++ci; continue; }
                if (c == '\'' && !dq) { sq = !sq; ++ci; continue; }
                if (c == '"'  && !sq) { dq = !dq; ++ci; continue; }
                if (c == '#' && !sq && !dq) { line = line.substr(0, ci); break; }
                ++ci;
            }
        }
        if (line.empty()) continue;

        // Alias expansion (recursive, max depth 5)
        for (int depth = 0; depth < 5; ++depth)
        {
            std::istringstream iss(line);
            std::string first;
            if (!(iss >> first)) break;
            auto it = aliases_.find(first);
            if (it == aliases_.end()) break;
            line = it->second + line.substr(first.size());
        }

        // Pre-dispatch --help
        auto toks = cli_tokenize(line, env_, last_exit_, dir_.listChildNames());
        if (!toks.empty() && (toks[0] == "--help" || toks[0] == "-h"))
        {
            if (toks.size() > 1)
            {
                auto it = help_text_.find(toks[1]);
                if (it != help_text_.end()) std::cout << it->second << "\n";
                else                        std::cout << "No help for " << toks[1] << "\n";
                last_exit_ = 0;
            }
            else
            {
                for (auto& kv : help_text_) std::cout << kv.second << "\n";
                std::cout << "Shell:  ;  &&  ||  |  >  >>   $VAR  \"...\"  '...'  *  ?\n";
                last_exit_ = 0;
            }
            continue;
        }

        std::vector<ShellCmd> cmds = parse(line);
        execute(cmds);
        disk_.dumpFile();
    }
    disk_.dumpFile();
}

int CLI::builtin_test_(const std::vector<std::string>& a)
{
    if (a.empty()) return 1;
    auto cond = [&](const std::string& op, const std::string& name) -> int
    {
        if (op == "-e") return (dir_.isFile(name) || dir_.isDir(name)) ? 0 : 1;
        if (op == "-d") return dir_.isDir(name) ? 0 : 1;
        if (op == "-f") return dir_.isFile(name) ? 0 : 1;
        if (op == "-r") { auto* f = dir_.getFile(name); return f ? 0 : 1; }
        if (op == "-w") { auto* f = dir_.getFile(name); return f && f->canWrite ? 0 : 1; }
        if (op == "-s") { auto* f = dir_.getFile(name); return f && f->size > 0 ? 0 : 1; }
        if (op == "-z") return name.empty() ? 0 : 1;
        return 1;
    };
    if (a.size() == 2) return cond(a[0], a[1]);
    if (a.size() == 3)
    {
        if (a[1] == "=")  return a[0] == a[2] ? 0 : 1;
        if (a[1] == "!=") return a[0] != a[2] ? 0 : 1;
    }
    return 1;
}

// ===========================================================================
// parse + execute (shell-level operators)
// ===========================================================================

void CLI::execute(std::vector<ShellCmd>& cmds)
{
    for (size_t i = 0; i < cmds.size(); ++i)
    {
        ShellCmd& sc = cmds[i];
        if (sc.argv.empty())
        {
            if (sc.op == '&' || sc.op == '|') last_exit_ = 1;
            continue;
        }

        bool should_run = true;
        if (i > 0)
        {
            char prev_op = cmds[i - 1].op;
            if (prev_op == '&' && last_exit_ != 0) should_run = false;
            if (prev_op == '|' && last_exit_ ==  0) should_run = false;
        }
        if (!should_run) continue;

        // Pipe
        if (sc.op == '|' && i + 1 < cmds.size() && !cmds[i + 1].argv.empty())
        {
            std::ostringstream oss;
            auto* old = std::cout.rdbuf(oss.rdbuf());
            last_exit_ = dispatch(sc.argv[0], argv_slice_(sc.argv));
            std::cout.rdbuf(old);

            cmds[i + 1].pipe_in_ = oss.str();
            continue;
        }

        // Redirection
        if (sc.op == '>' || sc.op == 'A')
        {
            std::ostringstream oss;
            auto* old = std::cout.rdbuf(oss.rdbuf());
            last_exit_ = dispatch(sc.argv[0], argv_slice_(sc.argv));
            std::cout.rdbuf(old);
            if (!sc.out_file.empty())
            {
                std::ofstream out(sc.out_file, sc.op == 'A' ? std::ios::app : std::ios::trunc);
                out << oss.str();
            }
            continue;
        }

        last_exit_ = dispatch(sc.argv[0], argv_slice_(sc.argv), sc.pipe_in_);
    }
}

CLI::ShellCmd CLI::make_cmd_(const std::string& line)
{
    ShellCmd sc;
    sc.argv = cli_tokenize(line, env_, last_exit_, dir_.listChildNames());
    sc.op = ';';
    sc.out_file = "";
    sc.pipe_in_ = "";
    return sc;
}

std::vector<CLI::ShellCmd> CLI::parse(const std::string& line)
{
    std::vector<ShellCmd> cmds;
    std::string cur;
    bool sq = false, dq = false, esc = false;
    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (esc) { esc = false; cur += c; continue; }
        if (c == '\\') { esc = true; cur += c; continue; }
        if (c == '\'' && !dq) { sq = !sq; cur += c; continue; }
        if (c == '"'  && !sq) { dq = !dq; cur += c; continue; }
        if (sq || dq) { cur += c; continue; }

        // >>
        if (c == '>' && i + 1 < line.size() && line[i + 1] == '>')
        {
            ShellCmd sc; std::string rest(line.begin() + i + 2, line.end());
            sc.argv = argv_first_tok_(rest);
            sc.op = 'A';
            sc.out_file = sc.argv.empty() ? "" : sc.argv[0];
            sc.argv.clear();
            cmds.push_back(make_cmd_(cur));
            cmds.push_back(sc);
            break;
        }
        if (c == '>')
        {
            ShellCmd sc; std::string rest(line.begin() + i + 1, line.end());
            sc.argv = argv_first_tok_(rest);
            sc.op = '>';
            sc.out_file = sc.argv.empty() ? "" : sc.argv[0];
            sc.argv.clear();
            cmds.push_back(make_cmd_(cur));
            cmds.push_back(sc);
            break;
        }
        if (c == '|')
        {
            cmds.push_back(make_cmd_(cur));
            cmds.back().op = '|';
            cur.clear();
            continue;
        }
        if (c == ';')
        {
            cmds.push_back(make_cmd_(cur));
            cur.clear();
            continue;
        }
        if (c == '&' && i + 1 < line.size() && line[i + 1] == '&')
        {
            cmds.push_back(make_cmd_(cur));
            cmds.back().op = '&';
            cur.clear();
            ++i;
            continue;
        }
        cur += c;
    }
    if (!cur.empty()) cmds.push_back(make_cmd_(cur));
    return cmds;
}

std::vector<std::string> CLI::argv_first_tok_(const std::string& s)
{
    std::vector<std::string> out;
    std::string tok;
    for (char c : s)
    {
        if (std::isspace(static_cast<unsigned char>(c)))
        { if (!tok.empty()) { out.push_back(tok); tok.clear(); break; } continue; }
        tok += c;
    }
    if (!tok.empty()) out.push_back(tok);
    return out;
}

std::vector<std::string> CLI::argv_slice_(const std::vector<std::string>& v)
{
    return v.size() > 1 ? std::vector<std::string>(v.begin() + 1, v.end())
                        : std::vector<std::string>();
}

int CLI::dispatch(const std::string& cmd, const std::vector<std::string>& args, const std::string& pipeIn)
{
    auto it = branch_table_.find(cmd);
    if (it != branch_table_.end())
    {
        if (!pipeIn.empty())
        {
            std::istringstream iss(pipeIn);
            auto* old = std::cin.rdbuf(iss.rdbuf());
            int r = it->second(args);
            std::cin.rdbuf(old);
            return r;
        }
        return it->second(args);
    }
    std::cout << red(cmd) << ": command not found\n";
    return 127;
}

// ===========================================================================
// UI
// ===========================================================================

void CLI::refresh()
{
    std::string path = dir_.GetPath(dir_.getWorkDir());
    std::cout << "["
              << bold(green(dir_.getUserName()))
              << "@"
              << cyan("simuos")
              << " "
              << bold(yellow(path))
              << "]"
              << bold(white("$ "))
              << std::flush;
}

void CLI::print_name_(const std::string& name)
{
    if (dir_.isDir(name))     std::cout << blue(bold(name)) << "  ";
    else if (dir_.isFile(name)) std::cout << white(name) << "  ";
    else                       std::cout << "No such file: " << name << "\n";
}

void CLI::ls_all()
{
    auto names = dir_.listChildNames();
    if (names.empty()) return;
    int ncols = 5;
    int maxw = 0;
    for (auto& n : names) maxw = std::max(maxw, static_cast<int>(n.size()));
    ++maxw;
    std::string line;
    int col = 0;
    for (auto& n : names)
    {
        std::string s = dir_.isDir(n) ? blue(bold(n)) : white(n);
        std::string padded = s + std::string(maxw - static_cast<int>(n.size()), ' ');
        line += padded;
        ++col;
        if (col >= ncols) { std::cout << line << "\n"; line.clear(); col = 0; }
    }
    if (!line.empty()) std::cout << line << "\n";
}

void CLI::ls_long(const std::string& name)
{
    bool list_all = (name == "__ALL__");
    auto names = list_all ? dir_.listChildNames() : std::vector<std::string>{name};
    for (auto& n : names)
    {
        if (dir_.isDir(n))
        {
            std::cout << "drwxr-xr-x  1 " << dir_.getUserName() << " "
                      << dir_.getUserName() << "       0 "
                      << std::asctime(std::localtime(nullptr))
                      << blue(bold(n)) << "\n";
        }
        else
        {
            auto* f = dir_.getFile(n);
            if (!f) { std::cout << white(n) << ": No such file\n"; continue; }
            std::string perms;
            for (int i = 0; i < 3; ++i)
            {
                perms += (f->canRead  ? "r" : "-");
                perms += (f->canWrite ? "w" : "-");
            }
            std::cout << "-" << perms << "  1 " << f->owner << " "
                      << dir_.getUserName() << "  "
                      << std::setw(6) << f->size << " "
                      << std::asctime(std::localtime(&f->create_time))
                      << white(f->name) << "\n";
        }
    }
}

void CLI::du_dir()
{
    auto names = dir_.listChildNames();
    long total = 0;
    for (auto& n : names)
    {
        auto* f = dir_.getFile(n);
        if (f)
        {
            int blocks = (f->size + DiskManager::BLOCK_SIZE - 1) / DiskManager::BLOCK_SIZE;
            int kb = blocks * DiskManager::BLOCK_SIZE / 1024;
            std::cout << std::setw(4) << kb << "\t" << white(n) << "\n";
            total += kb;
        }
    }
    std::cout << std::setw(4) << total << "\t" << bold("total") << "\n";
}

std::string CLI::human_size(int bytes)
{
    if (bytes < 0) return "-1";
    if (bytes < 1024)              return std::to_string(bytes) + "B";
    if (bytes < 1024 * 1024)       return std::to_string(bytes / 1024) + "K";
    return std::to_string(bytes / (1024 * 1024)) + "M";
}