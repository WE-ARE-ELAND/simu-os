#include <iostream>
#include "cli.h"
#include "threadManage.h"

int main()
{
    ThreadManager tm;
    CLI cli(tm, tm.getDIR(), tm.getDIM(), tm.getMem());
    cli.showMenu();
    tm.joinAllThreads();
    return 0;
}
