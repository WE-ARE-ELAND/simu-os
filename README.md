# SimuOS — Operating Systems Simulation

A user-space simulation of core OS subsystems (disk, directory, memory, thread management) for OS coursework. Not a real kernel.

## Stack

- **Language**: C++11
- **Build**: GNU Make (`makefile`)
- **Compiler**: MinGW-w64 (g++)
- **Libraries**: STL only — `<thread>`, `<mutex>`, `<vector>`, `<unordered_map>`, `<fstream>`, `<random>`, etc. No third-party dependencies.
- **Encoding**: Pure ASCII / English. No GBK flags needed.

## Quick Start

```bash
make -f makefile        # build → build/launch
make -f makefile run    # build and run
make -f makefile tests  # build test binaries
make -f makefile clean  # clean build artifacts
```

```bash
./build/launch
```

## Glass Terminal (optional)

SimuOS looks best in a modern terminal with frosted-glass / transparency support.

```powershell
powershell -ExecutionPolicy Bypass -File install-glass.ps1
```

This registers SimuOS as a Windows Terminal profile with Acrylic blur + 60% opacity.

Full guide: [`.terminal/README.md`](.terminal/README.md)

## Default Accounts

| Account | Password | Role |
|---------|----------|------|
| admin   | admin    | root |
| user1   | 123456   | user |
| user2   | 123456   | user |
| user3   | 123456   | user |
| user4   | 123456   | user |

## Commands

SimuOS has a Bash-like shell with argument parsing, colored output, env vars, aliases, and shell operators.

### Filesystem

| Command | Description |
|---------|-------------|
| `mkdir <dir...>` | Create directory(ies) |
| `rmdir <dir...>` | Remove empty directory(ies) |
| `cd <path>` | Change directory |
| `ls [-l] [<name>...]` | List directory (color-coded: blue=dir, white=file) |
| `ls -l` | Long format with perms/owner/size/date |
| `rm <file...>` | Delete file(s) (background thread) |
| `touch <file>` | Create file (background data-gen thread) |
| `tree` | Directory tree |
| `readfile <file>` | Read file into memory (LRU page replacement) |
| `write <file>` | Append content to file |
| `rename <old> <new>` | Rename file or directory |
| `stat <file>` | Detailed file metadata |

### POSIX Builtins

| Command | Description |
|---------|-------------|
| `pwd` | Print working directory |
| `whoami` | Print current user |
| `date` | Print current date/time |
| `echo [-n] [arg...]` | Print arguments |
| `clear` | Clear the screen |
| `history` | Show command history |
| `alias <name>=<cmd>` | Define / list aliases |
| `unalias <name>` | Remove an alias |
| `export KEY=VALUE` | Set environment variable |
| `env` | List environment variables |
| `type <name>` | Show command type (alias/builtin/file) |
| `test <expr>` / `[ <expr> ]` | Evaluate condition (`-e -d -f -r -w -s -z`, `=`, `!=`) |
| `help [cmd]` | Show help |

### GNU-style

| Command | Description |
|---------|-------------|
| `du` | Disk usage in current dir |
| `df` | Filesystem space usage |
| `free` | Memory + swap usage |
| `who` | Logged-in user |
| `stat <file>` | File stats |

### System

| Command | Description |
|---------|-------------|
| `readDisk <block>` | Read raw disk block |
| `disk` | Show free-block superblock (grouped free list) |
| `refresh` | Redraw prompt |
| `quit` | Exit |

## Shell Features

```bash
# Environment variables + expansion
export USER=admin
echo $USER        # → admin
echo ${USER}      # → admin
echo $?           # → last exit code

# Aliases (recursive expansion, max depth 5)
alias ll='ls -l'
ll

# Shell operators
ls; pwd                           # chain
test -d x && echo yes             # conditional (success)
test -d y || echo no              # conditional (failure)
ls > /tmp/out.txt                 # output redirection
ls >> /tmp/out.txt                # append redirection
ls | cat                          # pipe (stdout→stdin)

# Quotes + escaping
echo "hello world"                # double quotes (expands $VAR)
echo 'literal $VAR'               # single quotes (no expansion)
echo hello\ world                 # backslash escape

# Globbing
ls *                              # match any
ls ?.txt                          # match single char

# Comments
echo hello # this line ignored
```

## Readline

- **Up / Down**: Navigate command history
- **Left / Right**: Cursor movement
- **Home / End**: Jump to line start / end
- **Tab**: Complete file/directory names (common prefix, or show candidates)
- **Ctrl-C**: Clear current line
- **Ctrl-D**: Delete char at cursor, or exit on empty line

## Key Bindings

| Key | Action |
|-----|--------|
| Up / Down | History navigate |
| Left / Right | Cursor |
| Home / End | Line start / end |
| Tab | Auto-complete |
| Backspace | Delete char |
| Ctrl-C | Clear line |
| Ctrl-D | Delete char / exit |

## Module Overview

- **DiskManager**: FAT-based file organization with grouped free-block list (`disk.txt`, 40 KB image, blocks 900–1023 = swap area)
- **DirectoryManage**: In-memory tree structure with file metadata (owner, permissions, create time, disk address)
- **MemoryManager**: 2560 bytes / 64 blocks / 40 bytes each, LRU page replacement, swap I/O to disk blocks 900–1023
- **ThreadManager**: Real `std::thread` for concurrent data generation, deletion, and execution

## Data Flow

```
touch file
  → data_generation_thread → CreateFile()
  → AllocateBlocks() → CreateDirEntry()

readfile file
  → execute_thread → ReadFile() → allocateThreads()
  → LRU eviction: pageOut() → writeSwapBlock()
  → pageIn() → readSwapBlock() (on demand)

quit
  → dumpFile() → disk.txt
```

## Architecture

```
launch.cpp → ThreadManager
                ├── DirectoryManage  (tree, metadata)
                ├── DiskManager      (FAT, free blocks, swap I/O)
                ├── MemoryManager    (page tables, LRU, swap)
                └── CLI              (readline, dispatch, shell)
```

## Code Quality

- **Bug fixes**: All 17 critical/high bugs from the original code are fixed (use-after-free, double-free, array bounds, missing swap I/O, non-thread-safe `rand()`, phantom map entries, recursive login)
- **Const correctness**: All non-mutating methods marked `const`
- **RAII**: Smart-pointer ownership via `unique_ptr` throughout the directory tree
- **Thread safety**: `std::lock_guard` everywhere; shared input mutex between CLI and worker threads; `thread_local std::mt19937` for RNG
- **Const references**: `const std::string&` for all string parameters

## Tests

```bash
make -f makefile tests
./build/test_disk.exe
./build/test_directory.exe
./build/test_memory.exe
```

Test programs are interactive; follow the menu prompts.

## Documentation

- Disk management: [`docs/diskManageReadMe.md`](docs/diskManageReadMe.md)
- Directory management: [`docs/DirectoryManageReadMe.md`](docs/DirectoryManageReadMe.md)
- Task sheet: [`docs/操作系统课程设计任务书.md`](docs/操作系统课程设计任务书.md)
- Demo script: [`docs/演示文稿.md`](docs/演示文稿.md)
- Glass terminal: [`.terminal/README.md`](.terminal/README.md)
