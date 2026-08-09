# SimuOS — 操作系统模拟课程设计

在用户空间模拟实现操作系统核心子系统（**磁盘管理、目录管理、内存管理、线程管理**），作为操作系统课程的教学演示，**并非真实的操作系统内核**。

## 技术栈

- **语言**：C++11
- **构建**：GNU Make（`makefile`）
- **编译器**：MinGW-w64 (g++)
- **标准库**：仅用 STL（`<thread>` `<mutex>` `<vector>` `<unordered_map>` `<fstream>` `<random>` 等），无第三方依赖
- **编码**：纯 ASCII / 英文输出，无需 GBK 转码参数

## 项目结构

```
simu-os/
├── src/                    # 源代码
│   ├── launch.cpp          # 入口：创建 ThreadManager / CLI 并运行
│   ├── threadManage.cpp    # 线程管理（数据生成 / 删除 / 执行线程）
│   ├── cli.cpp             # 命令行菜单与 Bash-like 命令分发
│   ├── directoryManage.cpp # 目录管理（树形目录、文件元数据）
│   ├── diskManage.cpp      # 磁盘管理（FAT、成组链接法、兑换区）
│   └── memoryManage.cpp    # 内存管理（页表、LRU 置换、换出到兑换区）
├── include/                # 头文件（各模块接口）
├── tests/                  # 各模块独立测试程序
│   ├── test_disk.cpp
│   ├── test_directory.cpp
│   └── test_memory.cpp
├── build/                  # 编译输出
├── disk.txt                # 磁盘镜像文件（40960 字节）
├── docs/                   # 文档
├── .terminal/              # 毛玻璃终端配置
│   └── README.md
├── install-glass.ps1       # 一键配置 Windows Terminal 毛玻璃效果
└── makefile
```

## 快速开始

```bash
make -f makefile          # 编译主程序 → build/launch
make -f makefile run      # 编译并运行
make -f makefile tests    # 编译三个测试程序
make -f makefile clean    # 清理编译产物
```

```bash
./build/launch
```

启动后先登录（默认账号见下），然后按提示输入命令。

## 毛玻璃半透明终端（推荐）

在 Windows Terminal 中获得**毛玻璃模糊 + 半透明**效果：

```powershell
powershell -ExecutionPolicy Bypass -File install-glass.ps1
```

一键注册 SimuOS 为 Windows Terminal 的一个 profile，开启：
- Acrylic 毛玻璃模糊：**ON**
- 透明度：**60%**
- 配色：One Half Dark

完整教程：[`.terminal/README.md`](.terminal/README.md)

## 默认账号

| 账号 | 密码 | 角色 |
|------|------|------|
| admin | admin | 管理员 |
| user1 | 123456 | 普通用户 |
| user2 | 123456 | 普通用户 |
| user3 | 123456 | 普通用户 |
| user4 | 123456 | 普通用户 |

## 支持的命令

SimuOS 的 shell 支持参数、彩色输出、环境变量、别名、Shell 操作符，体验接近 Bash。

### 文件系统

| 命令 | 功能 |
|------|------|
| `mkdir <目录...>` | 创建目录 |
| `rmdir <目录...>` | 删除空目录 |
| `cd <路径>` | 切换目录（支持 `..` 和相对路径） |
| `ls [-l] [<名称>...]` | 显示目录内容（蓝色=目录，白色=文件） |
| `ls -l` | 长格式：权限 / 所有者 / 大小 / 日期 |
| `rm <文件...>` | 删除文件（后台线程） |
| `touch <文件>` | 创建文件（后台数据生成线程） |
| `tree` | 显示目录树 |
| `readfile <文件>` | 读文件到内存（LRU 页置换） |
| `write <文件>` | 追加写文件 |
| `rename <旧名> <新名>` | 重命名文件 / 目录 |
| `stat <文件>` | 显示文件详细元数据 |

### POSIX 内置命令

| 命令 | 功能 |
|------|------|
| `pwd` | 打印当前工作目录 |
| `whoami` | 打印当前用户名 |
| `date` | 打印当前日期时间 |
| `echo [-n] [参数...]` | 输出参数 |
| `clear` | 清屏 |
| `history` | 显示命令历史 |
| `alias <名>=<命令>` | 定义 / 列出别名 |
| `unalias <名>` | 删除别名 |
| `export KEY=VALUE` | 设置环境变量 |
| `env` | 列出环境变量 |
| `type <名>` | 显示命令类型（别名 / 内置 / 文件） |
| `test <表达式>` / `[ <表达式> ]` | 条件判断（`-e -d -f -r -w -s -z`, `=`, `!=`） |
| `help [命令]` | 显示帮助 |

### GNU 风格命令

| 命令 | 功能 |
|------|------|
| `du` | 当前目录磁盘用量 |
| `df` | 文件系统空间使用情况 |
| `free` | 内存 + 兑换区使用情况 |
| `who` | 显示登录用户 |
| `stat <文件>` | 文件统计信息 |

### 系统命令

| 命令 | 功能 |
|------|------|
| `readDisk <块号>` | 读取指定磁盘块内容 |
| `disk` | 查看空闲盘块（成组链接法超级块） |
| `refresh` | 刷新路径提示 |
| `quit` | 退出 |

## Shell 特性

```bash
# 环境变量 + 展开
export USER=admin
echo $USER        # → admin
echo ${USER}      # → admin
echo $?           # → 上一条命令退出码

# 别名（递归展开，最大深度 5）
alias ll='ls -l'
ll

# Shell 操作符
ls; pwd                            # 顺序执行
test -d x && echo yes              # 成功时执行
test -d y || echo no               # 失败时执行
ls > /tmp/out.txt                  # 输出重定向
ls >> /tmp/out.txt                 # 追加重定向
ls | cat                           # 管道（stdout→stdin）

# 引号 + 转义
echo "hello world"                 # 双引号（会展开 $VAR）
echo 'literal $VAR'                # 单引号（不展开）
echo hello\ world                  # 反斜杠转义

# 通配符
ls *                               # 匹配任意
ls ?.txt                           # 匹配单个字符

# 注释
echo hello # 注释部分会被忽略
```

## 键盘快捷键

| 键 | 功能 |
|----|------|
| ↑ / ↓ | 历史命令上下切换 |
| ← / → | 光标左右移动 |
| Home / End | 跳到行首 / 行尾 |
| Tab | 自动补全文件名 |
| Backspace | 删除字符 |
| Ctrl-C | 清空当前行 |
| Ctrl-D | 删除字符 / 空行时退出 |

## 模块说明

- **磁盘管理**（DiskManager）：`disk.txt` 模拟磁盘，FAT 显式链接组织文件，成组连接法管理空闲块，900–1023 块为兑换区。详见 [`docs/diskManageReadMe.md`](docs/diskManageReadMe.md)。
- **目录管理**（DirectoryManage）：内存中树形目录（不占磁盘块），含文件元数据（所有者、创建时间、磁盘地址、读写权限）。详见 [`docs/DirectoryManageReadMe.md`](docs/DirectoryManageReadMe.md)。
- **内存管理**（MemoryManager）：2560 字节模拟内存，64 块 × 40 字节，每线程 8 块，LRU 页面置换，换出页写入磁盘兑换区。
- **线程管理**（ThreadManager）：用真实 `std::thread` 模拟并发进程（数据生成 / 删除 / 执行），`std::mutex` 保护内存访问。

## 数据流

```
touch file
  → data_generation_thread → CreateFile()
  → AllocateBlocks() → CreateDirEntry()

readfile file
  → execute_thread → ReadFile() → allocateThreads()
  → LRU 置换: pageOut() → writeSwapBlock()
  → pageIn() → readSwapBlock()（按需读回）

quit
  → dumpFile() → disk.txt
```

## 架构

```
launch.cpp → ThreadManager
                ├── DirectoryManage  （树形目录，元数据）
                ├── DiskManager      （FAT，空闲块，兑换区 I/O）
                ├── MemoryManager    （页表，LRU，换出换入）
                └── CLI              （readline，分发，shell）
```


## 测试

```bash
make -f makefile tests
./build/test_disk.exe
./build/test_directory.exe
./build/test_memory.exe
```

测试程序为交互菜单，按提示输入数字操作即可。

## 文档

- 任务书：[`docs/操作系统课程设计任务书.md`](docs/操作系统课程设计任务书.md)
- 磁盘管理：[`docs/diskManageReadMe.md`](docs/diskManageReadMe.md)
- 目录管理：[`docs/DirectoryManageReadMe.md`](docs/DirectoryManageReadMe.md)
- 演示脚本：[`docs/演示文稿.md`](docs/演示文稿.md)
- 毛玻璃终端：[`.terminal/README.md`](.terminal/README.md)
