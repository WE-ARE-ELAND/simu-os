#pragma once

#include <bits/stdc++.h>

using namespace std;

/*----------定义磁盘块-------------*/
const int DISK_SIZE = 40 * 1024; // 磁盘大小为 40KB
const int BLOCK_SIZE = 40;       // 每个磁盘块的大小为 40B，块内寻址需要6bit
const int NUM_BLOCKS = 1024;     // 磁盘块总数为 1024 个，块间寻址需要10bit
const int NUM_SWAP_BLOCKS = 124; // 用于存储兑换区的磁盘块数为 124 个
const int NUM_FILE_BLOCKS = 900; // 用于存储文件的磁盘块数为 900 个

struct DiskBlock
{
    int BlockNum; // 块号
    string data;  // 数据
};

/*----------定义文件分配表的结构-------------*/
// 表示下一块的块号; 下一块为-1，代表无下一块; -2表示这个磁盘块空闲
