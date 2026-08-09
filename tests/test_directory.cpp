#include "directoryManage.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main() {
	DirectoryManage DIR;
	DIR.login();
	int sel;
	cout << "Enter command number (not the command name): \n"
		<< "1.md 2.rd 3.cd 4.dir 5.del 6.create 7.tree 8.readFile 9.writeFile 10.rename 11.exit\n";
	while(true){
		string path = "";
		path=DIR.GetPath(DIR.getWorkDir());
		cout << path<<">:";
		cin >> sel;
		switch (sel) {
			case 1:
			{
				string name;
				cout << "Enter directory name: ";
				cin >> name;
				DIR.MakeDir(name);
				break;
			}

			case 2: {
				string name;
				cout << "Enter directory name: ";
				cin >> name;
				DIR.rd(name);
				break;
			}
			case 3: {
				string name;
				cout << "Enter path: ";
				cin >> name;
				DIR.cdDir(name);
				break;
			}
			case 4: {
				DIR.dir();
				break;
			}
			case 5: {
				string name;
				cout << "Enter file name: ";
				cin >> name;
				DIR.DeleteFile(name);
				break;
			}
			case 6: {
				string name;
				cout << "Enter file name: ";
				cin >> name;
				DIR.CreateFile(name);
				break;
			}
			case 7:
			{
				DIR.tree(DIR.getWorkDir(), -1);
				break;
			}
			case 8: {
				string path;
				cout << "Enter file path: ";
				cin >> path;
				DIR.ReadFile(path);
				break;
			}
			case 9: {
				string name;
				cout << "Enter file name: ";
				cin >> name;
				DIR.WriteFile(name);
				break;
			}
			case 10: {
				string oldName, newName;
				cout << "Enter old name and new name (separated by space): ";
				cin >> oldName;
				cin >> newName;
				DIR.ren(oldName, newName);
				break;
			}
			case 11:
				exit(0);
			default:
				cout << "Invalid option, exiting.\n";
				exit(0);
		}
	}
}