#include "DirectoryManage.h"


//构造函数
DirectoryManage::DirectoryManage()
{
	workDir = NULL;
	rootDir = NULL;
	path = "";
	// user = {"登陆失败", 0, "登陆失败", "登陆失败", 0};
    	user.name = "登陆失败";
   	user.userRight = 0;
    	user.account = "登陆失败";
    	user.password = "登陆失败";
    	user.state = 0;
}

//用户登陆函数
void DirectoryManage::login()
{
	string account;
	string password;
	cout << "---------登录目录管理系统---------\n";
	cout << "账号：";
	cin >> account;
	cout << "密码：";
	cin >> password;
	user = QueryUser(account, password);
	if (user.state == 0) {
		cout << "登陆失败！请重新登录！\n";
		system("pause");
		system("cls");
		login();
	}
	CreateRootDirectory();
	MakeDir(user.name);
}

//用户查询函数
DirectoryManage::User DirectoryManage::QueryUser(string account, string password)
{
	User userList[] = {
		{"admin",1,"admin","admin",0},
		{"user1",2,"user1","123456",0},
		{"user2",3,"user1","123456",0},
		{"user3",4,"user1","123456",0},
		{"user4",5,"user1","123456",0}
	};
	User FailUser = { "登陆失败",0,"登陆失败", "登陆失败", 0 };
	for (int i = 0; i < sizeof(userList)/sizeof(User); i++) {
		if (account == userList[i].account) {
			if (password == userList[i].password) {
				userList[i].state = 1;
				cout << "登陆成功！欢迎" << userList[i].name << "\n";
				return userList[i];
			}
			else {
				cout << "密码错误！\n";
				return FailUser;
			}
		}
	}
	cout << "账号不存在！\n";
	return FailUser;
}

//创建根目录
void DirectoryManage::CreateRootDirectory() {
	rootDir = new Directory();
	rootDir->name = "root";
	rootDir->firstChildDirectory = NULL;
	rootDir->firstChildFile = NULL;
	rootDir->lastChildFile = NULL;
	rootDir->frontDirectory = NULL;
	rootDir->nextDirectory = NULL;
	rootDir->parentDirectory = NULL;
	rootDir->canRead = 1;//根目录可读
	rootDir->canWrite = 0;//根目录不可写
	cout<<"创建根目录成功！\n";
	workDir = rootDir;
}

//tree显示当前目录的树形结构
void DirectoryManage::tree(Directory* curDir, int level)
{
	if (curDir == workDir) {
		cout << workDir->name<<"\n";
	}
	for (int i = 0; i < level; i++) {
		cout << "|      ";
	}
	if(curDir!=workDir) cout << "|--- " << curDir->name<<"\n";
	if (curDir->firstChildDirectory) {
		Directory* ptr = curDir->firstChildDirectory;
		while (ptr) {
			tree(ptr, level + 1);
			ptr = ptr->nextDirectory;
		}
	}
	if (curDir->firstChildFile) {
		DirectoryEntry* p = curDir->firstChildFile;
		while (p) {
			for (int i = 0; i < level+1; i++) {
				cout << "|      ";
			}
			cout << "|--- " << p->file.name << "\n";
			p = p->nextFile;
		}
	}
}

//md新建目录：新建一个空目录
void DirectoryManage::MakeDir(string name) {
	Directory* newDir = new Directory();
	newDir->name = name;
	newDir->firstChildDirectory = NULL;
	newDir->firstChildFile = NULL;
	newDir->lastChildFile = NULL;
	newDir->frontDirectory = NULL;
	newDir->nextDirectory = NULL;
	newDir->parentDirectory = workDir;
	newDir->canRead = user.userRight;
	newDir->canWrite = user.userRight;

	if (workDir->firstChildDirectory == NULL) {
		workDir->firstChildDirectory = newDir;
	}
	else {
		Directory* ptr = workDir->firstChildDirectory;
		while (true) {
			if (ptr->name == name) {
				cout << "该目录名称已存在，请修改名称后重新创建目录！\n";
				return;
			}
			if (ptr->nextDirectory) {
				ptr = ptr->nextDirectory;
			}
			else {
				ptr->nextDirectory = newDir;
				newDir->frontDirectory = ptr;
				break;
			}
		}
	}
	cout << "新目录"<<name<<"创建成功！\n";
}

//cd切换目录
void DirectoryManage::cdDir(string path)
{
	if (path == "..") {//返回父目录
		if (workDir->parentDirectory) workDir = workDir->parentDirectory;
		else cout << "当前目录已经是根目录\n";
		return;
	}
	else {//切换到当前目录某一路径下的子目录中
		queue<string> Path;
		string curPath="";
		for (int i = 0; i < (int)path.length(); i++) {
			if (path[i] != '\\' && path[i] != '/') {
				curPath += path[i];
			}
			else {
				Path.push(curPath);
				curPath = "";
			}
		}
		Path.push(curPath);
		queue<Directory*> q;
		q.push(workDir);
		while (!q.empty()) {
			Directory* p = q.front();
			q.pop();
			if (p->firstChildDirectory) {
				Directory* ptr = p->firstChildDirectory;
				if (ptr->name == Path.front()) {
					Path.pop();
					if (Path.empty()) {
						workDir = ptr;
						return;
					}
					else {
						q.push(ptr);
					}
				}
				while (ptr->nextDirectory) {
					ptr = ptr->nextDirectory;
					if (ptr->name == Path.front()) {
						Path.pop();
						if (Path.empty()) {
							workDir = ptr;
							return;
						}
						else {
							q.push(ptr);
						}
					}
				}
			}
		}
	}
	cout << "路径输入有误！\n";
}

//rd删除目录：删除当前目录中的子目录
void DirectoryManage::rd(string name) {
	if (workDir->firstChildDirectory == NULL) {
		cout << "当前目录无子目录，不存在你要删除的目录！\n";
		return;
	}
	Directory* p = workDir->firstChildDirectory;
	while (p) {
		if (p->name == name) {
			if (workDir->firstChildDirectory == p) {
				workDir->firstChildDirectory = p->nextDirectory;
				if (p->nextDirectory) p->nextDirectory->frontDirectory = NULL;
				DeleteDir(p);
			}
			else {
				p->frontDirectory->nextDirectory = p->nextDirectory;
				if (p->nextDirectory) p->nextDirectory->frontDirectory = p->frontDirectory;
				DeleteDir(p);
			}
			cout << "目录'" << name << "'删除成功！\n";
			return;
		}
		p = p->nextDirectory;
	}
	cout << "不存在'" << name << "'目录\n";
}
//删除目录中所有的文件及文件夹
void DirectoryManage::DeleteDir(Directory* p)
{
	if (p) {
		if (p->firstChildDirectory) {
			Directory* ptr = p->firstChildDirectory;
			while (ptr->nextDirectory) {
				Directory* q = ptr->nextDirectory;
				DeleteDir(ptr);
				ptr = q;
			}
			DeleteDir(ptr);
		}
		if (p->firstChildFile) {
			DirectoryEntry* ptr = p->firstChildFile;
			while (ptr->nextFile) {
				DirectoryEntry* q = ptr->nextFile;
				delete(ptr);
				ptr = q;
			}
		}
		delete(p);
		p = NULL;
	}
}


//dir命令
void DirectoryManage::dir()
{
	cout << "请选择功能：\n"
		<< "1.显示当前目录下所有文件夹和文件\n"
		<< "2.显示当前目录下某文件的信息\n"
		<< "3.显示当前目录下某一子目录的信息\n";
	int sel;
	cin >> sel;
	if (sel == 1) {
		ListAll();
	}
	else if (sel == 2) {
		cout << "请输入文件名:";
		string name;
		cin >> name;
		dirFile(name);
	}
	else if (sel == 3) {
		cout << "请输入目录的相对路径：";
		string dir_path="";
		cin >> dir_path;
		dirPath(dir_path);
	}
}
//显示当前目录下所有文件夹和文件
void DirectoryManage::ListAll() {
	cout << "----------------------------------------------------------------\n";
	queue<Directory*> q;
	q.push(workDir);
	while (!q.empty()) {
		Directory* p = q.front();
		q.pop();
		if (p->firstChildDirectory || p->firstChildFile) {
			GetPath(p);
			cout << path << "下子目录及文件\n";
		}
		if (p->firstChildDirectory) {
			Directory *ptr = p->firstChildDirectory;
			cout << '\t' << ptr->name << '\t';
			cout << "<DIR>\n";
			q.push(ptr);
			while (ptr->nextDirectory) {
				ptr = ptr->nextDirectory;
				cout << '\t' << ptr->name << '\t';
				cout << "<DIR>\n";
				q.push(ptr);
			}
		}
		if (p->firstChildFile) {
			DirectoryEntry *ptr = p->firstChildFile;
			cout << '\t' << ptr->file.name << '\t';
			cout << "<FILE>\n";
			while (ptr->nextFile) {
				ptr = ptr->nextFile;
				cout << '\t' << ptr->file.name << '\t';
				cout << "<FILE>\n";
			}
		}
	}
	cout << "----------------------------------------------------------------\n";
}
//显示当前目录下某文件的信息
void DirectoryManage::dirFile(string name) {
	if (workDir->firstChildFile) {
		DirectoryEntry* ptr = workDir->firstChildFile;
		while (ptr) {
			if (ptr->file.name == name) {
				char dt[30];
				ctime_s(dt, sizeof dt, &ptr->file.create_time);
				cout << "创建时间：" << dt<<"\n";
				cout << "文件名：" << ptr->file.name << "\n";
				cout << "文件大小：" << ptr->file.size<<"\n";
				cout << "文件属性：";
				if (ptr->file.canRead == user.userRight || ptr->file.canRead == 1) {
					cout << "可读  ";
				}
				else cout << "不可读  ";
				if (ptr->file.canWrite == user.userRight || ptr->file.canWrite == 1) {
					cout << "可写\n";
				}
				else cout << "不可写\n";
				return;
			}
			ptr = ptr->nextFile;
		}
	}
	cout << "不存在该文件！\n";
}
//显示某子目录的信息
void DirectoryManage::ListDir(Directory* p)
{
	if (p->canRead != user.userRight && p->canRead != 1) {
		cout << "不好意思，您没有权限读取该目录的信息！\n";
		return;
	}
	if (p->firstChildDirectory || p->firstChildFile) {
		GetPath(p);
		cout << path << "下子目录及文件\n";
	}
	else {
		cout << "当前目录为空目录！\n";
		return;
	}
	if (p->firstChildDirectory) {
		p = p->firstChildDirectory;
		cout << '\t' << p->name << '\t';
		cout << "<DIR>\n";
		while (p->nextDirectory) {
			p = p->nextDirectory;
			cout << '\t' << p->name << '\t';
			cout << "<DIR>\n";
		}
	}
	if (p->firstChildFile) {
		DirectoryEntry* ptr = p->firstChildFile;
		cout << '\t' << ptr->file.name << '\t';
		cout << "<FILE>\n";
		while (ptr->nextFile) {
			ptr = ptr->nextFile;
			cout << '\t' << ptr->file.name << '\t';
			cout << "<FILE>\n";
		}
	}
}
//显示相对路径下某子目录的信息
void DirectoryManage::dirPath(string path)
{
	queue<string> Path;
	string curPath = "";
	for (int i = 0; i < (int)path.length(); i++) {
		if (path[i] != '\\' && path[i] != '/') {
			curPath += path[i];
		}
		else {
			Path.push(curPath);
			curPath = "";
		}
	}
	Path.push(curPath);
	queue<Directory*> q;
	q.push(workDir);
	while (!q.empty()) {
		Directory* p = q.front();
		q.pop();
		if (Path.empty()) {
			ListDir(p);
			return;
		}
		if (p->firstChildDirectory) {
			Directory* ptr = p->firstChildDirectory;
			if (ptr->name == Path.front()) {
				Path.pop();
				q.push(ptr);
				continue;
			}
			while (ptr->nextDirectory) {
				ptr = ptr->nextDirectory;
				if (ptr->name == Path.front()) {
					Path.pop();
					q.push(ptr);
					continue;
				}
			}
		}
	}
	cout << "路径输入有误或不存在该目录！\n";
}

//ren命令
void DirectoryManage::ren(string name, string newName)
{
	//重命名目录
	if (workDir->firstChildDirectory) {
		Directory* p = workDir->firstChildDirectory;
		if (p->name == name) {
			RenameDir(p, newName);
			return;
		}
	}
	//重命名文件
	if (workDir->firstChildFile) {
		DirectoryEntry* q = workDir->firstChildFile;
		if (q->file.name == name) {
			RenameFile(q, newName);
			return;
		}
	}
}
//重命名目录
void DirectoryManage::RenameDir(Directory* ptr,string newName) {
	//权限判断
	if (ptr->canWrite!=1&&ptr->canWrite!=user.userRight) {
		cout << "用户权限不足，无法修改！\n";
		return;
	}
	if(workDir->firstChildDirectory){
		//重命名判断
		Directory* p = workDir->firstChildDirectory;
		while (p) {
			if (p != ptr&&p->name == newName) {
				cout << "该目录名称已存在，无法重命名该目录！\n";
				return;
			}
			p = p->nextDirectory;
		}
		ptr->name = newName;
		cout << "目录名修改成功！新目录名为" << ptr->name << "\n";
		return;
	}
}
//重命名文件
void DirectoryManage::RenameFile(DirectoryEntry* ptr,string newName) {
	//权限判断
	if (ptr->file.canWrite != 1 && ptr->file.canWrite != user.userRight) {
		cout << "用户权限不足，无法修改！\n";
		return;
	}
	if(workDir->firstChildFile) {
		//重名判断
		DirectoryEntry* p = workDir->firstChildFile;
		while (p) {
			if (p!=ptr&&p->file.name == newName) {
				cout << "该文件名称已存在，无法重命名该文件！\n";
				return;
			}
			p = p->nextFile;
		}
		ptr->file.name = newName;
		cout << "文件名修改成功！新文件名为" << ptr->file.name << "\n";
		return;
	}
}

//获取当前文件夹的路径
string DirectoryManage::GetPath(Directory* curDir) {
	path = "";
	stack<Directory*> q;
	q.push(curDir);
	while (curDir->parentDirectory) {
		q.push(curDir->parentDirectory);
		curDir = curDir->parentDirectory;
	}
	while (!q.empty()) {
		Directory* p = q.top();
		q.pop();
		path += p->name;
		if (!q.empty()) path += "\\";
	}
	return path;
}

//create创建文件
DirectoryManage::File* DirectoryManage::CreateFile(string name) {
	//检查文件是否重名
	DirectoryEntry* p = workDir->firstChildFile;
	while (p) {
		if (p->file.name == name) {
			cout << "该文件名称已存在，请更改文件名后重新创建文件！\n";
			return NULL;
		}
		p = p->nextFile;
	}
	//创建文件
	File* newFile=new File();
	newFile->name = name;
	cout << "请输入文件内容：\n";
	getchar();
	getline(cin,newFile->context);
	newFile->size = newFile->context.length();
	time(&newFile->create_time);//创建文件的时间
	newFile->structure = "text";//文件结构默认文本文件
	newFile->owner = user.name;
	newFile->canRead = user.userRight;
	newFile->canWrite = user.userRight;
	return newFile;
	//执行数据生成线程,传递的参数有File newFile
	//thread t(data_generation_thread); // 运行到这边就开一个新的线程执行data_generation_thread函数
	//t.join();
	//CreateDirEntry(*newFile, 4);
}
//为文件建立目录项
void DirectoryManage::CreateDirEntry(File file, int first_block) {
	DirectoryEntry* newDirEntry = new DirectoryEntry();
	newDirEntry->file = file;
	newDirEntry->first_block = first_block;
	newDirEntry->frontFile = workDir->lastChildFile;
	newDirEntry->nextFile = NULL;
	newDirEntry->parentDirectory = workDir;
	if (workDir->firstChildFile==NULL) {
		workDir->firstChildFile = newDirEntry;
	}
	if (workDir->lastChildFile) {
		workDir->lastChildFile->nextFile = newDirEntry;
	}
	workDir->lastChildFile = newDirEntry;
}


//del删除文件
void DirectoryManage::DeleteFile(string name) {
	if (workDir->firstChildFile) {
		DirectoryEntry* ptr = workDir->firstChildFile;
		while (ptr) {
			if (ptr->file.name == name) {
				if (ptr == workDir->firstChildFile) {
					workDir->firstChildFile = ptr->nextFile;
					if(ptr->nextFile) ptr->nextFile->frontFile = NULL;
					delete ptr;
				}
				else if (ptr == workDir->lastChildFile) {
					workDir->lastChildFile = ptr->frontFile;
					ptr->frontFile->nextFile = ptr->nextFile;
				}
				else {
					ptr->frontFile->nextFile = ptr->nextFile;
					ptr->nextFile->frontFile = ptr->frontFile;
					delete ptr;
				}
				cout << "文件"<<name<<"删除成功！\n";
				return;
			}
			ptr = ptr->nextFile;
		}
	}
	cout << "不存在该文件！\n";
}

//type显示文件内容，可读文件时才能读
DirectoryManage::File* DirectoryManage::ReadFile(string path) {
	//读取文件相对路径
	queue<string> Path;
	string curPath = "";
	for (int i = 0; i < (int)path.length(); i++) {
		if (path[i] != '\\' && path[i] != '/') {
			curPath += path[i];
		}
		else {
			Path.push(curPath);
			curPath = "";
		}
	}
	Path.push(path);
	//读取文件信息
	queue<Directory*> q;
	q.push(workDir);
	while (!q.empty()) {
		Directory* p = q.front();
		q.pop();
		if (Path.size() == 1) {
			if (workDir->firstChildFile) {
				DirectoryEntry* t = workDir->firstChildFile;
				while (t) {
					if (t->file.name == Path.front()) {
						//权限判断
						if (t->file.canRead != 1 && t->file.canRead != user.userRight) {
							cout << "用户权限不足，无法查看该文件内容！\n";
							return NULL;
						}
						//输出文件内容
						cout << t->file.context<<"\n";
						return &t->file;
					}
					t = t->nextFile;
				}
			}
		}
		
		if (p->firstChildDirectory) {
			Directory* ptr = p->firstChildDirectory;
			if (ptr->name == Path.front()) {
				Path.pop();
				q.push(ptr);
				
			}
			while (ptr->nextDirectory) {
				ptr = ptr->nextDirectory;
				if (ptr->name == Path.front()) {
					Path.pop();
					q.push(ptr);
				}
			}
		}
	}
	cout << "路径有误，或不存在该文件！\n";
	return NULL;
}

//echo写文件，可写文件时才能写
DirectoryManage::File* DirectoryManage::WriteFile(string name) {
	if (workDir->firstChildFile) {
		DirectoryEntry* p = workDir->firstChildFile;
		while (p) {
			if (p->file.name == name) {
				//权限判断
				if (p->file.canWrite != 1 && p->file.canWrite != user.userRight) {
					cout << "用户权限不足，无法编写该文件内容！\n";
					return NULL;
				}
				cout << "请输入文件内容：\n";
				string context;
				getchar();
				getline(cin, context);
				p->file.context += context;
				cout << "写文件成功！\n";
				return &p->file;
			}
			p = p->nextFile;
		}
	}
	cout << "不存在'" << name << "'文件！\n";
	return NULL;
}

//open文件的打开
DirectoryManage::File* DirectoryManage::OpenFile(string name) {
	int flag = 0;
	DirectoryEntry* p = NULL;
	if (workDir->firstChildFile) {
		p = workDir->firstChildFile;
		while (p) {
			if (p->file.name == name) {
				flag = 1;
				break;
			}
			p = p->nextFile;
		}
	}
	if (flag == 0) {
		cout << "不存在'" << name << "'文件\n";
		return NULL;
	}
	cout << "打开文件成功！\n";
	return &p->file;
}
//close文件的关闭
DirectoryManage::File* DirectoryManage::CloseFile(string name) {
	int flag = 0;
	DirectoryEntry* p = NULL;
	if (workDir->firstChildFile) {
		p = workDir->firstChildFile;
		while (p) {
			if (p->file.name == name) {
				flag = 1;
				break;
			}
			p = p->nextFile;
		}
	}
	if (flag == 0) {
		cout << "不存在'" << name << "'文件\n";
		return NULL;
	}
	cout << "关闭文件成功！\n";
	return &p->file;
}
