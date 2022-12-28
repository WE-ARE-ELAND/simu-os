#include <thread>
#include "memoryManage.h"
#include "directoryManage.h"
#include "diskManage.h"

using namespace std;
// Mutex and condition variable for synchronizing access to memory
std::mutex memory_mutex;
std::condition_variable memory_cv;
DirectoryManage directoryManger;

// Function prototypes
void data_generation_thread(File newFile);
void delete_data_thread();
void execute_thread();
void visualize_memory();