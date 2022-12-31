#include "memoryManage.h"
#include <chrono>
using namespace std;
using namespace chrono;
// #include <windows.h>
#include <iostream>
// #include <chrono>
using namespace std::chrono;

int main()
{
    MemoryManager manager;
    manager.allocateThreads("the first thread", "content");
    manager.allocateThreads("main.cpp", "aaaaaaaaaaaaaaaaaaaaaaaaafffffffffaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    manager.deleteBlock("main.cpp");
    return 0;
}
