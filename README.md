# SimuOS — 操作系统模拟课程设计

在用户空间模拟实现操作系统核心子系统（**磁盘管理、目录管理、内存管理、线程管理**），作为操作系统课程的教学演示，**并非真实的操作系统内核**。

## 技术栈

- **语言**：C++11
- **构建**：GNU Make（`makefile`）
- **编译器**：MinGW-w64 (g++)
- **标准库**：仅用 STL（`<thread>` `<mutex>` `<vector>` `<unordered_map>` `<fstream>` 等），无第三方依赖

## 项目结构

```
simu-os/
├── src/                    # 源代码
│   ├── launch.cpp          # 入口：创建 ThreadManager / CLI 并运行
│   ├── threadManage.cpp    # 线程管理（数据生成 / 删除 / 执行线程）
│   ├── cli.cpp             # 命令行菜单与命令分发
│   ├── directoryManage.cpp # 目录管理（树形目录、文件元数据）
│   ├── diskManage.cpp      # 磁盘管理（FAT、成组链接法、兑换区）
│   └── memoryManage.cpp    # 内存管理（页表、LRU 置换）
├── include/                # 头文件（各模块接口）
├── tests/                  # 各模块独立测试程序
│   ├── test_disk.cpp
│   ├── test_directory.cpp
│   └── test_memory.cpp
├── build/                  # 编译输出（launch / 测试可执行文件）
├── disk.txt                # 磁盘镜像文件（40960 字节，运行生成 / 更新）
├── docs/                   # 文档
│   ├── diskManageReadMe.md
│   ├── DirectoryManageReadMe.md
│   ├── 操作系统课程设计任务书.md
│   └── 演示文稿.md
└── makefile                # 构建脚本
```

## 快速开始

```bash
make -f makefile          # 编译主程序 → build/launch
make -f makefile run      # 编译并运行
make -f makefile tests    # 编译三个测试程序
make -f makefile clean    # 清理编译产物
```

## 运行方式

```bash
./build/launch
```

启动后先登录（默认账号密码见下），然后按提示输入命令。

### 默认账号

| 账号 | 密码 | 角色 |
|---|---|---|
| admin | admin | 管理员 |
| user1 | 123456 | 普通用户 |
| user2 | 123456 | 普通用户 |
| user3 | 123456 | 普通用户 |
| user4 | 123456 | 普通用户 |

## 支持的命令

| 命令 | 功能 |
|---|---|
| `mkdir` | 创建目录 |
| `rmdir` | 删除目录 |
| `cd` | 切换目录（支持 `..` 与相对路径） |
| `ls` | 显示目录 / 文件信息（分 1 / 2 / 3 子菜单） |
| `rm` | 删除文件（后台线程） |
| `touch` | 创建文件（后台数据生成线程） |
| `tree` | 显示目录树 |
| `readfile` | 读文件（执行线程，调入内存并显示页表 / 磁盘内容） |
| `write` | 写文件（追加内容） |
| `rename` | 重命名文件 / 目录 |
| `readDisk` | 读取指定磁盘块内容 |
| `disk` | 查看空闲盘块（成组链接法超级块） |
| `refresh` | 刷新当前路径提示 |
| `quit` | 退出 |

> `rm` 与 `touch` 均通过后台线程执行，主线程不会阻塞，可继续输入其他命令。

## 编码说明

控制台程序的中文字符串按 **GBK** 编码输出，以适配中文 Windows 的 cmd.exe（代码页 936）。
若你在 UTF-8 终端（PowerShell 7 / Windows Terminal / 现代 IDE）运行显示异常，可在 `makefile` 中移除 `-fexec-charset=gbk` 改为 UTF-8。

## 模块说明

- **磁盘管理**（DiskManager）：`disk.txt` 模拟磁盘，FAT 显式链接组织文件，成组连接法管理空闲块，900–1023 块为兑换区。详见 `docs/diskManageReadMe.md`。
- **目录管理**（DirectoryManage）：内存中树形目录（不占磁盘块），含文件元数据（所有者、创建时间、磁盘地址、读写权限）。详见 `docs/DirectoryManageReadMe.md`。
- **内存管理**（MemoryManager）：2560 字节模拟内存，64 块 × 40 字节，每线程 8 块，LRU 页面置换，换出页写入磁盘兑换区。
- **线程管理**（ThreadManager）：用真实 `std::thread` 模拟并发进程（数据生成 / 删除 / 执行），`std::mutex` 保护内存访问。

## 数据流

1. `touch` → 数据生成线程 → `CreateFile()` → `AllocateBlocks()` → `CreateDirEntry()`
2. `readfile` → 执行线程 → `ReadFile()` → `allocateThreads()`，内存满时 LRU 换页到兑换区
3. 退出时 `dumpFile()` 把磁盘镜像写回 `disk.txt`

## 测试

```bash
make -f makefile tests
./build/test_disk.exe
./build/test_directory.exe
./build/test_memory.exe
```

> 注意：测试程序为交互脚本，可按菜单提示输入数字操作。

## 文档

- 任务书：`docs/操作系统课程设计任务书.md`
- 磁盘管理：`docs/diskManageReadMe.md`
- 目录管理：`docs/DirectoryManageReadMe.md`
- 演示脚本：`docs/演示文稿.md`
