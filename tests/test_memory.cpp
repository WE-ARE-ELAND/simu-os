#include "memoryManage.h"
#include <iostream>
using namespace std;

int main()
{
    MemoryManager manager;
    manager.allocateThreads("the first thread", "content");
    manager.allocateThreads("main.cpp", "aaaaaaaaaaaaaaaaaaaaaaaaafffffffffaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    cout << endl;
    manager.showPage("main.cpp");
    manager.deleteBlock("main.cpp");
    return 0;
}
