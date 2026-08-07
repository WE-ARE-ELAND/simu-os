#include "directoryManage.h"

DirectoryManage::DirectoryManage()
    : workDir_(nullptr), rootDir_(nullptr), path_("")
{
    user_.name = "登陆失败";
    user_.userRight = 0;
    user_.account = "登陆失败";
    user_.password = "登陆失败";
    user_.state = 0;
}

DirectoryManage::~DirectoryManage()
{
    DeleteDir(rootDir_);
}

void DirectoryManage::login()
{
    std::string account, password;
    std::cout << "---------登录SimuOs操作系统---------\n";
    std::cout << "账号：";
    std::cin >> account;
    std::cout << "密码：";
    std::cin >> password;
    user_ = QueryUser(account, password);
    if (user_.state == 0) {
        std::cout << "登陆失败！请重新登录！\n";
        login();
    }
    CreateRootDirectory();
    MakeDir(user_.name);
}

DirectoryManage::User DirectoryManage::QueryUser(std::string account, std::string password)
{
    User userList[] = {
        {"admin", 1, "admin", "admin", 0},
        {"user1", 2, "user1", "123456", 0},
        {"user2", 3, "user2", "123456", 0},
        {"user3", 4, "user3", "123456", 0},
        {"user4", 5, "user4", "123456", 0}
    };
    User FailUser = {"登陆失败", 0, "登陆失败", "登陆失败", 0};
    for (int i = 0; i < static_cast<int>(sizeof(userList) / sizeof(User)); i++) {
        if (account == userList[i].account) {
            if (password == userList[i].password) {
                userList[i].state = 1;
                std::cout << "登陆成功！欢迎" << userList[i].name << "\n";
                return userList[i];
            } else {
                std::cout << "密码错误！\n";
                return FailUser;
            }
        }
    }
    std::cout << "账号不存在！\n";
    return FailUser;
}

void DirectoryManage::CreateRootDirectory()
{
    rootDir_ = new Directory();
    rootDir_->name = "root";
    rootDir_->canRead = 1;
    rootDir_->canWrite = 0;
    std::cout << "创建根目录成功！\n";
    workDir_ = rootDir_;
}

void DirectoryManage::tree(Directory* curDir, int level)
{
    if (curDir == workDir_)
        std::cout << curDir->name << "\n";
    for (int i = 0; i < level; i++)
        std::cout << "|      ";
    if (curDir != workDir_)
        std::cout << "|--- " << curDir->name << "\n";
    if (curDir->firstChildDirectory) {
        Directory* ptr = curDir->firstChildDirectory.get();
        while (ptr) { tree(ptr, level + 1); ptr = ptr->nextDirectory.get(); }
    }
    if (curDir->firstChildFile) {
        DirectoryEntry* p = curDir->firstChildFile.get();
        while (p) {
            for (int i = 0; i < level + 1; i++) std::cout << "|      ";
            std::cout << "|--- " << p->file.name << "\n";
            p = p->nextFile.get();
        }
    }
}

void DirectoryManage::MakeDir(std::string name)
{
    std::unique_ptr<Directory> newDir(new Directory());
    newDir->name = name;
    newDir->parentDirectory = workDir_;
    newDir->canRead = user_.userRight;
    newDir->canWrite = user_.userRight;
    if (workDir_->firstChildDirectory == nullptr) {
        workDir_->firstChildDirectory = std::move(newDir);
    } else {
        Directory* ptr = workDir_->firstChildDirectory.get();
        while (true) {
            if (ptr->name == name) {
                std::cout << "该目录名称已存在，请修改名称后重新创建目录！\n";
                return;
            }
            if (ptr->nextDirectory)
                ptr = ptr->nextDirectory.get();
            else {
                ptr->nextDirectory = std::move(newDir);
                ptr->nextDirectory->frontDirectory = ptr;
                break;
            }
        }
    }
    std::cout << "新目录" << name << "创建成功！\n";
}

void DirectoryManage::cdDir(std::string path)
{
    if (path == "..") {
        if (workDir_->parentDirectory)
            workDir_ = workDir_->parentDirectory;
        else
            std::cout << "当前目录已经是根目录\n";
        return;
    }
    std::queue<std::string> Path;
    std::string curPath;
    for (char c : path) {
        if (c != '\\' && c != '/')
            curPath += c;
        else {
            Path.push(curPath);
            curPath.clear();
        }
    }
    Path.push(curPath);
    std::queue<Directory*> q;
    q.push(workDir_);
    while (!q.empty()) {
        Directory* p = q.front();
        q.pop();
        if (p->firstChildDirectory) {
            Directory* ptr = p->firstChildDirectory.get();
            if (ptr->name == Path.front()) {
                Path.pop();
                if (Path.empty()) { workDir_ = ptr; return; }
                q.push(ptr);
            }
            while (ptr->nextDirectory) {
                ptr = ptr->nextDirectory.get();
                if (ptr->name == Path.front()) {
                    Path.pop();
                    if (Path.empty()) { workDir_ = ptr; return; }
                    q.push(ptr);
                }
            }
        }
    }
    std::cout << "路径输入有误！\n";
}

void DirectoryManage::rd(std::string name)
{
    if (workDir_->firstChildDirectory == nullptr) {
        std::cout << "当前目录无子目录，不存在你要删除的目录！\n";
        return;
    }
    Directory* p = workDir_->firstChildDirectory.get();
    while (p) {
        if (p->name == name) {
            Directory* prev = p->frontDirectory;
            if (prev) {
                prev->nextDirectory.reset();
                prev->nextDirectory = std::move(p->nextDirectory);
                if (p->nextDirectory)
                    p->nextDirectory->frontDirectory = prev;
            } else {
                workDir_->firstChildDirectory.reset();
                workDir_->firstChildDirectory = std::move(p->nextDirectory);
                if (p->nextDirectory)
                    p->nextDirectory->frontDirectory = nullptr;
            }
            DeleteDir(p);
            std::cout << "目录'" << name << "'删除成功！\n";
            return;
        }
        p = p->nextDirectory.get();
    }
    std::cout << "不存在'" << name << "'目录\n";
}

void DirectoryManage::DeleteDir(Directory* p)
{
    if (!p) return;
    if (p->firstChildDirectory) {
        Directory* ptr = p->firstChildDirectory.get();
        while (ptr) {
            Directory* next = ptr->nextDirectory.get();
            ptr->nextDirectory.reset();
            DeleteDir(ptr);
            ptr = next;
        }
        p->firstChildDirectory.reset();
    }
    if (p->firstChildFile) {
        DirectoryEntry* ptr = p->firstChildFile.get();
        while (ptr) {
            DirectoryEntry* next = ptr->nextFile.get();
            ptr->nextFile.reset();
            delete ptr;
            ptr = next;
        }
        p->firstChildFile.reset();
    }
    delete p;
}

void DirectoryManage::dir()
{
    std::cout << "请选择功能：\n"
              << "1.显示当前目录下所有文件夹和文件\n"
              << "2.显示当前目录下某文件的信息\n"
              << "3.显示当前目录下某一子目录的信息\n";
    int sel;
    std::cin >> sel;
    if (sel == 1)
        ListAll();
    else if (sel == 2) {
        std::string name;
        std::cin >> name;
        dirFile(name);
    } else if (sel == 3) {
        std::string path;
        std::cin >> path;
        dirPath(path);
    }
}

void DirectoryManage::ListAll()
{
    std::cout << "----------------------------------------------------------------\n";
    std::queue<Directory*> q;
    q.push(workDir_);
    while (!q.empty()) {
        Directory* p = q.front();
        q.pop();
        if (p->firstChildDirectory || p->firstChildFile) {
            GetPath(p);
            std::cout << path_ << "下子目录及文件\n";
        }
        if (p->firstChildDirectory) {
            Directory* ptr = p->firstChildDirectory.get();
            std::cout << '\t' << ptr->name << '\t' << "<DIR>\n";
            q.push(ptr);
            while (ptr->nextDirectory) {
                ptr = ptr->nextDirectory.get();
                std::cout << '\t' << ptr->name << '\t' << "<DIR>\n";
                q.push(ptr);
            }
        }
        if (p->firstChildFile) {
            DirectoryEntry* ptr = p->firstChildFile.get();
            std::cout << '\t' << ptr->file.name << '\t' << "<FILE>\n";
            while (ptr->nextFile) {
                ptr = ptr->nextFile.get();
                std::cout << '\t' << ptr->file.name << '\t' << "<FILE>\n";
            }
        }
    }
    std::cout << "----------------------------------------------------------------\n";
}

void DirectoryManage::dirFile(std::string name)
{
    if (workDir_->firstChildFile) {
        DirectoryEntry* ptr = workDir_->firstChildFile.get();
        while (ptr) {
            if (ptr->file.name == name) {
                std::cout << "创建时间：" << std::ctime(&ptr->file.create_time);
                std::cout << "文件名：" << ptr->file.name << "\n";
                std::cout << "文件大小：" << ptr->file.size << "\n";
                std::cout << "文件属性：";
                std::cout << ((ptr->file.canRead == user_.userRight ||
                               ptr->file.canRead == 1)
                              ? "可读  "
                              : "不可读  ");
                std::cout << ((ptr->file.canWrite == user_.userRight ||
                               ptr->file.canWrite == 1)
                              ? "可写\n"
                              : "不可写\n");
                return;
            }
            ptr = ptr->nextFile.get();
        }
    }
    std::cout << "不存在该文件！\n";
}

void DirectoryManage::ListDir(Directory* p)
{
    if (p->canRead != user_.userRight && p->canRead != 1) {
        std::cout << "不好意思，您没有权限读取该目录的信息！\n";
        return;
    }
    if (p->firstChildDirectory || p->firstChildFile) {
        GetPath(p);
        std::cout << path_ << "下子目录及文件\n";
    } else {
        std::cout << "当前目录为空目录！\n";
        return;
    }
    if (p->firstChildDirectory) {
        Directory* child = p->firstChildDirectory.get();
        std::cout << '\t' << child->name << '\t' << "<DIR>\n";
        while (child->nextDirectory) {
            child = child->nextDirectory.get();
            std::cout << '\t' << child->name << '\t' << "<DIR>\n";
        }
    }
    if (p->firstChildFile) {
        DirectoryEntry* ptr = p->firstChildFile.get();
        std::cout << '\t' << ptr->file.name << '\t' << "<FILE>\n";
        while (ptr->nextFile) {
            ptr = ptr->nextFile.get();
            std::cout << '\t' << ptr->file.name << '\t' << "<FILE>\n";
        }
    }
}

void DirectoryManage::dirPath(std::string path)
{
    std::queue<std::string> Path;
    std::string curPath;
    for (char c : path) {
        if (c != '\\' && c != '/')
            curPath += c;
        else {
            Path.push(curPath);
            curPath.clear();
        }
    }
    Path.push(curPath);
    std::queue<Directory*> q;
    q.push(workDir_);
    while (!q.empty()) {
        Directory* p = q.front();
        q.pop();
        if (Path.empty()) { ListDir(p); return; }
        if (p->firstChildDirectory) {
            Directory* ptr = p->firstChildDirectory.get();
            if (ptr->name == Path.front()) {
                Path.pop();
                q.push(ptr);
                continue;
            }
            while (ptr->nextDirectory) {
                ptr = ptr->nextDirectory.get();
                if (ptr->name == Path.front()) {
                    Path.pop();
                    q.push(ptr);
                    continue;
                }
            }
        }
    }
    std::cout << "路径输入有误或不存在该目录！\n";
}

void DirectoryManage::ren(std::string name, std::string newName)
{
    if (workDir_->firstChildDirectory) {
        Directory* p = workDir_->firstChildDirectory.get();
        while (p) {
            if (p->name == name) {
                RenameDir(p, newName);
                return;
            }
            p = p->nextDirectory.get();
        }
    }
    if (workDir_->firstChildFile) {
        DirectoryEntry* q = workDir_->firstChildFile.get();
        while (q) {
            if (q->file.name == name) {
                RenameFile(q, newName);
                return;
            }
            q = q->nextFile.get();
        }
    }
    std::cout << "不存在'" << name << "'文件或目录\n";
}

void DirectoryManage::RenameDir(Directory* ptr, std::string newName)
{
    if (ptr->canWrite != 1 && ptr->canWrite != user_.userRight) {
        std::cout << "用户权限不足，无法修改！\n";
        return;
    }
    Directory* p = workDir_->firstChildDirectory.get();
    while (p) {
        if (p != ptr && p->name == newName) {
            std::cout << "该目录名称已存在，无法重命名该目录！\n";
            return;
        }
        p = p->nextDirectory.get();
    }
    ptr->name = newName;
    std::cout << "目录名修改成功！新目录名为" << ptr->name << "\n";
}

void DirectoryManage::RenameFile(DirectoryEntry* ptr, std::string newName)
{
    if (ptr->file.canWrite != 1 && ptr->file.canWrite != user_.userRight) {
        std::cout << "用户权限不足，无法修改！\n";
        return;
    }
    DirectoryEntry* p = workDir_->firstChildFile.get();
    while (p) {
        if (p != ptr && p->file.name == newName) {
            std::cout << "该文件名称已存在，无法重命名该文件！\n";
            return;
        }
        p = p->nextFile.get();
    }
    ptr->file.name = newName;
    std::cout << "文件名修改成功！新文件名为" << ptr->file.name << "\n";
}

std::string DirectoryManage::GetPath(Directory* curDir)
{
    path_ = "";
    std::stack<Directory*> q;
    q.push(curDir);
    while (curDir->parentDirectory) {
        q.push(curDir->parentDirectory);
        curDir = curDir->parentDirectory;
    }
    while (!q.empty()) {
        Directory* p = q.top();
        q.pop();
        path_ += p->name;
        if (!q.empty())
            path_ += "\\";
    }
    return path_;
}

DirectoryManage::File DirectoryManage::CreateFile(std::string name)
{
    DirectoryEntry* p = workDir_->firstChildFile.get();
    while (p) {
        if (p->file.name == name) {
            std::cout << "该文件名称已存在，请更改文件名后重新创建文件！\n";
            return File{};
        }
        p = p->nextFile.get();
    }
    File newFile;
    newFile.name = name;
    std::cout << "请输入文件内容：\n";
    std::cin.ignore();
    std::getline(std::cin, newFile.context);
    newFile.size = static_cast<int>(newFile.context.length());
    std::time(&newFile.create_time);
    newFile.structure = "text";
    newFile.owner = user_.name;
    newFile.canRead = user_.userRight;
    newFile.canWrite = user_.userRight;
    return newFile;
}

void DirectoryManage::CreateDirEntry(File file, int first_block)
{
    auto newDirEntry = std::unique_ptr<DirectoryEntry>(new DirectoryEntry());
    newDirEntry->file = std::move(file);
    newDirEntry->first_block = first_block;
    newDirEntry->frontFile = workDir_->lastChildFile;
    newDirEntry->parentDirectory = workDir_;
    if (workDir_->firstChildFile == nullptr) {
        workDir_->firstChildFile = std::move(newDirEntry);
    } else {
        workDir_->lastChildFile->nextFile = std::move(newDirEntry);
    }
    workDir_->lastChildFile = newDirEntry.get();
}

bool DirectoryManage::DeleteFile(std::string name)
{
    if (workDir_->firstChildFile == nullptr)
        return false;
    DirectoryEntry* ptr = workDir_->firstChildFile.get();
    while (ptr) {
        if (ptr->file.name == name) {
            DirectoryEntry* prev = ptr->frontFile;
            if (prev) {
                prev->nextFile.reset();
                prev->nextFile = std::move(ptr->nextFile);
                if (ptr->nextFile)
                    ptr->nextFile->frontFile = prev;
            } else {
                workDir_->firstChildFile.reset();
                workDir_->firstChildFile = std::move(ptr->nextFile);
                if (ptr->nextFile)
                    ptr->nextFile->frontFile = nullptr;
            }
            if (ptr == workDir_->lastChildFile)
                workDir_->lastChildFile = prev;
            delete ptr;
            return true;
        }
        ptr = ptr->nextFile.get();
    }
    return false;
}

DirectoryManage::File* DirectoryManage::ReadFile(std::string path)
{
    std::queue<std::string> Path;
    std::string curPath;
    for (char c : path) {
        if (c != '\\' && c != '/')
            curPath += c;
        else {
            Path.push(curPath);
            curPath.clear();
        }
    }
    Path.push(curPath);
    std::queue<Directory*> q;
    q.push(workDir_);
    while (!q.empty()) {
        Directory* p = q.front();
        q.pop();
        if (Path.size() == 1) {
            if (workDir_->firstChildFile) {
                DirectoryEntry* t = workDir_->firstChildFile.get();
                while (t) {
                    if (t->file.name == Path.front()) {
                        if (t->file.canRead != 1 &&
                            t->file.canRead != user_.userRight)
                            return nullptr;
                        return &t->file;
                    }
                    t = t->nextFile.get();
                }
            }
        }
        if (p->firstChildDirectory) {
            Directory* ptr = p->firstChildDirectory.get();
            if (ptr->name == Path.front()) {
                Path.pop();
                q.push(ptr);
            }
            while (ptr->nextDirectory) {
                ptr = ptr->nextDirectory.get();
                if (ptr->name == Path.front()) {
                    Path.pop();
                    q.push(ptr);
                }
            }
        }
    }
    return nullptr;
}

DirectoryManage::File* DirectoryManage::WriteFile(std::string name)
{
    if (workDir_->firstChildFile) {
        DirectoryEntry* p = workDir_->firstChildFile.get();
        while (p) {
            if (p->file.name == name) {
                if (p->file.canWrite != 1 &&
                    p->file.canWrite != user_.userRight) {
                    std::cout << "用户权限不足，无法编写该文件内容！\n";
                    return nullptr;
                }
                std::cout << "请输入文件内容：\n";
                std::string context;
                std::cin.ignore();
                std::getline(std::cin, context);
                p->file.context += context;
                std::cout << "写文件成功！\n";
                return &p->file;
            }
            p = p->nextFile.get();
        }
    }
    std::cout << "不存在'" << name << "'文件！\n";
    return nullptr;
}

DirectoryManage::File* DirectoryManage::OpenFile(std::string name)
{
    if (workDir_->firstChildFile) {
        DirectoryEntry* p = workDir_->firstChildFile.get();
        while (p) {
            if (p->file.name == name) {
                std::cout << "打开文件成功！\n";
                return &p->file;
            }
            p = p->nextFile.get();
        }
    }
    std::cout << "不存在'" << name << "'文件\n";
    return nullptr;
}

DirectoryManage::File* DirectoryManage::CloseFile(std::string name)
{
    if (workDir_->firstChildFile) {
        DirectoryEntry* p = workDir_->firstChildFile.get();
        while (p) {
            if (p->file.name == name) {
                std::cout << "关闭文件成功！\n";
                return &p->file;
            }
            p = p->nextFile.get();
        }
    }
    std::cout << "不存在'" << name << "'文件\n";
    return nullptr;
}
