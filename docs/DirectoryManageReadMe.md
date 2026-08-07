# 目录管理

## 命令行接口

当前程序通过 `CLI` 类提供命令行接口，命令采用 Linux 风格命名，由 `cli.cpp` 分发到 `DirectoryManage` 及各线程。

| 命令 | 功能 | 对应方法 |
|---|---|---|
| `mkdir` | 在当前目录创建子目录 | `MakeDir(name)` |
| `rmdir` | 删除当前目录下的子目录（含其内容） | `rd(name)` |
| `cd` | 切换目录，支持 `..` 与相对路径 | `cdDir(path)` |
| `ls` | 显示目录 / 文件信息（分 1 / 2 / 3 子菜单） | `dir()` / `dirFile()` / `dirPath()` |
| `rm` | 删除文件（启动删除数据线程） | `DeleteFile()` + 回收磁盘 |
| `touch` | 创建文件（启动数据生成线程） | `CreateFile()` |
| `tree` | 显示目录树 | `tree(curDir, level)` |
| `readfile` | 读文件（启动执行线程，调入内存） | `ReadFile()` |
| `write` | 写文件（追加内容） | `WriteFile(name)` |
| `rename` | 重命名文件或目录 | `ren(old, new)` |
| `readDisk` | 读取指定磁盘块内容 | `DIM.readBlock(num)` |
| `disk` | 查看空闲盘块（成组链接法） | `DIM.printFreeBlocks()` |
| `refresh` | 刷新当前路径提示 | — |
| `quit` | 退出系统 | — |

> `rm` 与 `touch` 均通过后台线程执行，主线程不会阻塞，可继续输入其他命令。

### 登录

程序启动后要求登录，默认账号见 README。目录权限（`canRead` / `canWrite`）与用户权限等级比较，控制读写与重命名。

### 目录结构

树形结构。每个目录持有子目录链表（`unique_ptr`）与子文件链表（`unique_ptr` + 反向指针），并记录父目录指针。遍历采用队列 / 栈，支持路径分段解析（`/` 与 `\` 均可作为分隔符）。

## 数据结构

### File（文件元数据）

| 字段 | 类型 | 说明 |
|---|---|---|
| `name` | `string` | 文件名 |
| `context` | `string` | 文件内容 |
| `size` | `int` | 文件大小（字节） |
| `structure` | `string` | 文件结构 / 类型（如 `"text"`） |
| `create_time` | `time_t` | 创建时间 |
| `owner` | `string` | 所有者 |
| `canRead` / `canWrite` | `int` | 读 / 写权限（与用户权限等级比较） |

### DirectoryEntry（目录项 / 文件控制块 FCB）

| 字段 | 说明 |
|---|---|
| `file` | 关联的 File 元数据 |
| `first_block` | 文件在磁盘中的首块号 |
| `frontFile` | 同级目录中的上一个文件（反向指针） |
| `nextFile` | 同级目录中的下一个文件（`unique_ptr`） |
| `parentDirectory` | 所属父目录 |

### Directory（目录）

| 字段 | 说明 |
|---|---|
| `name` | 目录名 |
| `nextDirectory` | 同级下一目录（`unique_ptr`） |
| `frontDirectory` | 同级上一目录 |
| `parentDirectory` | 父目录 |
| `firstChildDirectory` | 首个子目录（`unique_ptr`） |
| `firstChildFile` / `lastChildFile` | 首 / 末子文件 |
| `canRead` / `canWrite` | 目录读 / 写权限 |

## 目录类接口

- **创建根目录**：`CreateRootDirectory()`，首次登录时自动创建
- **新建目录**：`MakeDir(name)`，在当前工作目录下创建空目录，同名则拒绝
- **删除目录**：`rd(name)` / `DeleteDir(p)`，递归删除目录及其子目录、子文件
- **切换目录**：`cdDir(path)`，支持 `..` 与多段相对路径
- **显示目录**：`dir()` / `ListAll()` / `ListDir(p)` / `dirFile()` / `dirPath()`，分情况显示全部、某路径、某文件信息
- **重命名**：`ren(old, new)` / `RenameDir` / `RenameFile`，检查权限与重名
- **路径**：`GetPath(curDir)`，返回从根到当前目录的完整路径（`\\` 分隔）
- **创建文件**：`CreateFile(name)`，输入内容并初始化元数据，返回 `File`
- **建立目录项**：`CreateDirEntry(file, first_block)`，由数据生成线程调用
- **删除文件**：`DeleteFile(name)`，删除目录项（磁盘回收由删除数据线程完成）
- **读文件**：`ReadFile(path)`，权限校验后返回文件指针
- **写文件**：`WriteFile(name)`，权限校验后追加内容
- **打开 / 关闭文件**：`OpenFile(name)` / `CloseFile(name)`

## 对外接口

- **命令行接口**：通过 `CLI` 类在控制台直接操作目录（`mkdir` / `rmdir` / `cd` / `ls` / `tree` / `touch` / `readfile` / `write` / `rename` 等）
- **类接口**：`DirectoryManage` 提供上述方法供 `ThreadManager` 与各线程调用
- 目录本身**不占用磁盘块**，仅作为内存中的元数据结构存在
