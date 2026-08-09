#include "diskManager.h"

using namespace std;

int main()
{
    DiskManager DIM;

    cout << "1. Allocate block 2. Free block\n 3. Write data 4. Read data\n 5. Create file 6. Delete file\n 7. Write swap 8. Read swap\n 9. View free blocks 10. Exit" << endl;
    while (true)
    {
        int sel;
        cin >> sel;
        switch (sel)
        {
        case 1:
        {
            short num_block = DIM.allocateBlock();
            cout << "Allocated block: " << num_block;
            break;
        }
        case 2:
        {
            cout << "Enter block number to free: " << endl;
            short num_block;
            cin >> num_block;
            if (DIM.freeBlock(num_block) == 1)
            {
                cout << num_block << " freed successfully.\n" << endl;
                break;
            }
            cout << "This block is already free." << endl;
            break;
        }
        case 3:
        {
            cout << "Enter block number to write: " << endl;
            short num_block;
            cin >> num_block;
            string data;
            cout << "Enter data to write: " << endl;
            cin >> data;
            DIM.writeBlock(num_block, data);
            break;
        }
        case 4:
        {
            cout << "Enter block number to read: " << endl;
            short num_block;
            cin >> num_block;
            string data = DIM.readBlock(num_block);
            cout << data;
            break;
        }
        case 5:
        {
            string name, data;
            cout << "File name: " << endl;
            cin >> name;
            int size;
            cout << "Enter file size: " << endl;
            cin >> size;
            cout << "Enter file data: " << endl;
            cin >> data;
            if (DIM.AllocateBlocks(name, size, data) == 1)
            {
                cout << "File allocated. First block: " << DIM.getFirstBlock(name) << endl;
                break;
            }
            cout << "Allocation failed: insufficient space." << endl;
            break;
        }
        case 6:
        {
            cout << "Enter file name to delete: " << endl;
            string name;
            cin >> name;
            DIM.DeallocateBlocks(name);
            cout << "Deleted successfully." << endl;
            break;
        }
        case 7:
        {
            cout << "Enter swap block number to write: " << endl;
            short num_block;
            cin >> num_block;
            string data;
            cout << "Enter data to write: " << endl;
            cin >> data;
            DIM.writeSwapBlock(num_block, data);
            break;
        }
        case 8:
        {
            cout << "Enter swap block number to read: " << endl;
            short num_block;
            cin >> num_block;
            string data;
            cout << "Enter data to write: " << endl;
            cin >> data;
            DIM.readSwapBlock(num_block, data);
            cout << data;
            break;
        }
        case 9:
        {
            DIM.printFreeBlocks();
            break;
        }
        case 10:
        {
            string fileName;
            cout << "Enter file name to query: " << endl;
            cin >> fileName;
            cout << DIM.ReadFileDataFromDisk(fileName) << endl;
            break;
        }
        case 11:
        {
            string fileName;
            cout << "Enter file name to query: " << endl;
            cin >> fileName;
            for (int i : DIM.getAllBlocks(fileName))
            {
                cout << i << " ";
            }
            cout << endl;
            break;
        }
        default:
            exit(0);
        }
    }
    DIM.dumpFile();
    return 0;
}