# 目录管理

## 目录结构

树形结构，用队列来实现对树形结构的各种操作

## File//文件结构体

- string name//文件名
- string context//文件内容
- int size//文件大小
- int structure//文件结构

## FCB//文件控制块（目录项）

 定义一个文件控制块的结构体，包含

- File file//文件结构体
- string owner//文件所有者
- time create_time//创建时间
- int first_block//在磁盘中存放的地址
- struct File* frontFile//同级目录上一文件
- struct File* nextFile//同级目录下一文件
- struct Directory* parentDirectory//父目录

## Directory//目录结构体

- name//目录名
- struct Directory* nextDirectory//同级下一目录
- struct Directory* frontDirectory//同级上一目录
- struct Directory* parentDirectory//父目录
- struct Directory* firstChildDirectory//子目录
- struct File* firstChildFile//首个子文件
- struct File* lastChildFile//最后一个子文件
- int canRead//读权限
- int canWrite//写权限

## 目录类

### 变量定义

- 当前工作目录
- 根目录
- 路径

### 方法定义

- 创建根目录
- 新建目录：新建一个空目录
- 删除目录：删除目录及目录中的文件
- 显示当前目录下所有文件夹
- 修改目录：重命名
- 获取当前文件夹的路径
- 创建文件：在此方法中输入数据大小、数据信息、存储目录、文件名等，然后调用数据生成线程来创建文件。
- 为文件建立目录项
- 删除文件：删除文件的目录项
- 读文件：可读文件时才能读
- 写文件：可写文件时才能写
- 修改文件：重命名或修改文件权限
- 文件的打开
- 文件的关闭

## 对外接口

- 命令行接口：直接调用cmd窗口来实现目录管理
- 图形化界面：直接在现有的图形界面中实现目录管理
