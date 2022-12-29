#include "DirectoryManage.h"

int main() {
	DirectoryManage DIR;
	DIR.login();
	int sel;
	cout << "请输入你要实现的功能(输入数字，不要输入命令本身！！！)：\n"
		<< "1.md 2.rd 3.cd 4.dir 5.del 6.create 7.tree 8.readFile 9.writeFile 10.rename 11.exit\n";
	while(true){
		string path = "";
		path=DIR.GetPath(DIR.workDir);
		cout << path<<">:";
		cin >> sel;
		switch (sel) {
			case 1:
			{
				string name;
				cout << "请输入目录名：";
				cin >> name;
				DIR.MakeDir(name);
				break;
			}
		
			case 2: {
				string name;
				cout << "请输入目录名：";
				cin >> name;
				DIR.rd(name);
				break;
			}
			case 3: {
				string name;
				cout << "请输入路径：";
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
				cout << "请输入文件名：";
				cin >> name;
				DIR.DeleteFile(name);
				break;
			}
			case 6: {
				string name;
				cout << "请输入文件名：";
				cin >> name;
				DIR.CreateFile(name);
				break;
			}
			case 7:
			{
				DIR.tree(DIR.workDir, -1);
				break;
			}
			case 8: {
				string path;
				cout << "请输入文件名所在路径：";
				cin >> path;
				DIR.ReadFile(path);
				break;
			}
			case 9: {
				string name;
				cout << "请输入文件名：";
				cin >> name;
				DIR.WriteFile(name);
				break;
			}
			case 10: {
				string oldName, newName;
				cout << "请输入旧文件名和新文件名，中间以空格间隔：";
				cin >> oldName;
				cin >> newName;
				DIR.ren(oldName, newName);
				break;
			}
			case 11:
				exit(0);
			default:exit(0);
		}
	}
}
