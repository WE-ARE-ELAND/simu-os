# 磁盘管理 (DiskManager)

## 概述

磁盘管理模块用一个固定大小的二进制文件模拟磁盘，提供磁盘块分配 / 回收、FAT 文件链组织、成组连接法空闲块管理，以及内存-磁盘兑换（swap）区读写。

- **磁盘镜像文件**：`disk.txt`（40960 字节）
- **块大小**：`BLOCK_SIZE = 40` 字节（见 `diskManager.h`）
- **总块数**：`NUM_BLOCKS_ = 1024`

> 注：任务书原始规格为 4KB 磁盘 / 每块 4 字节。本实现为放大比例演示，每块 40 字节、总容量 40960 字节，**算法逻辑与规格完全一致**，便于观察。

### 块区划分

| 区间 | 块号 | 用途 | 大小 |
|---|---|---|---|
| 0–51 | `NUM_FAT_BLOCKS_ = 52` | FAT 表 + 元数据区 | 52 块 |
| 52–899 | `NUM_FILE_BLOCKS_ = 900` | 文件数据区（可分配） | 900 块 |
| 900–1023 | `NUM_SWAP_BLOCKS_ = 124` | 兑换区（内存换页使用） | 124 块 |

空闲块范围：52–1023，共 **972** 块，由 `FreeDataBlockNum_`（文件区）与 `FreeSwapBlockNum_`（兑换区）分别统计。

## 数据结构

### 磁盘块 `DiskBlock`

```cpp
struct DiskBlock { int BlockNum; std::string data; };
```

`MyDisk_[NUM_BLOCKS_]` 用数组模拟整张磁盘，`data` 长度固定为 `BLOCK_SIZE`，空闲块填充 `'#'`。

### FAT 表 `fatList_[NUM_BLOCKS_]`

`short` 数组，记录文件块的显式链接链：

- **`-2`**：该块为空闲块
- **`-1`**：链尾（文件的最后一个块）
- **`>= 0`**：指向下一个磁盘块号

文件链通过 `fileNameToNumOfBlock_`（`unordered_map<string, short>`）由文件名定位到首块，再沿 FAT 链遍历全部块。

### 成组连接法空闲块表

`freeBlocks_[NUM_FREE_GROUPS_][NUM_FREE_PER_GROUP_ + 2]`

`NUM_FREE_GROUPS_ = 10` 组，每组最多 `NUM_FREE_PER_GROUP_ = 100` 块。每组布局：

- `[0]`：本组当前空闲块数
- `[1]`：下一组组号（`0` 表示无后续组）
- `[2]` 起：本组空闲块块号列表

超级块（第 0 组）始终驻留内存；分配时优先从第 0 组取，取空则把下一组载入。

### 空闲块计数器

- `FreeDataBlockNum_`：文件区（52–899）空闲块数
- `FreeSwapBlockNum_`：兑换区（900–1023）空闲块数
- `allocateBlock()` 按分配的块号所属区间自动递减对应计数器，`freeBlock()` 反之递增

## FAT 显式链接

创建文件时 `AllocateBlocks()` 依次 `allocateBlock()` 取出若干块，按 FAT 链串起：首块由 `fileNameToNumOfBlock_` 记录，后续块由前一块的 `fatList_` 指向，末块置 `-1`。

删除文件时 `DeallocateBlocks()` 从首块沿 FAT 链逐块 `freeBlock()` 回收，最后从文件名映射表移除该文件。

## 兑换区（Swap Area）

块号 900–1023 为兑换区，供 `MemoryManager` 在 LRU 页面置换时使用：换出页写入兑换区，需要时再换入。兑换区**不通过 `AllocateBlocks` 自动分配**，由内存管理模块通过 `writeSwapBlock` / `readSwapBlock` 直接读写。

## 成组连接法流程

**分配**（`allocateBlock()`）：
1. 若第 0 组 `freeNum > 0`，从 `[freeNum + 1]` 取出一块，`freeNum--`
2. 否则用 `[1]` 载入下一组到第 0 组，再取一块
3. 第 0 组为空且无下一组时返回 `-1`（分配失败）

**回收**（`freeBlock(short num_block)`）：
1. 块数据清空为 `'#'`
2. 若该块 FAT 为 `-2`（已空闲），返回 `-1`
3. 找到未满的组，把块号追加到组尾，`freeNum++`
4. 重置 FAT 为 `-2`，递增对应计数器

## 对外接口

| 函数 | 说明 |
|---|---|
| `short allocateBlock()` | 按成组连接法分配一块空闲块，返回块号；失败返回 `-1` |
| `int freeBlock(short num_block)` | 回收一块块号，重置为空闲并更新 FAT / 计数器；重复回收返回 `-1` |
| `void writeBlock(short num_block, string data)` | 向磁盘块写入数据 |
| `string readBlock(short block_num)` | 从磁盘块读取数据 |
| `int AllocateBlocks(string fileName, int size, string data)` | 为文件分配 `size` 块并写入数据，成功返回 `1`，空间不足返回 `-1` |
| `void DeallocateBlocks(string fileName)` | 删除文件并回收其所有磁盘块 |
| `string ReadFileDataFromDisk(string fileName)` | 沿 FAT 链读回文件全部数据 |
| `vector<int> getAllBlocks(string fileName)` | 返回文件占用的所有块号 |
| `short getFirstBlock(string fileName)` | 返回文件首块号 |
| `void writeSwapBlock(short blockNum, string buffer)` | 向兑换区写入 |
| `void readSwapBlock(short blockNum, string &buffer)` | 从兑换区读取 |
| `void printFreeBlocks()` | 打印成组连接法超级块信息 |
| `void dumpFile()` | 把内存中的磁盘镜像写回 `disk.txt` |
| `void PrintMyDisk()` | 打印整张磁盘内容（调试用） |

## 注意

- 所有对外接口直接操作内存中的 `MyDisk_` 数组，`dumpFile()` 才落盘到 `disk.txt`
- `BLOCK_SIZE = 40`，写入数据会按块截断
- 兑换区（900–1023）仅供内存置换使用，不参与文件数据分配
- 程序每次命令处理都会调用 `dumpFile()` 保存状态，退出时再次保存
