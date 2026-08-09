#include "directoryManage.h"

DirectoryManage::DirectoryManage()
    : workDir_(nullptr), rootDir_(nullptr), path_("")
{
    user_.name = "login_failed";
    user_.userRight = 0;
    user_.account = "login_failed";
    user_.password = "login_failed";
    user_.state = 0;
}

DirectoryManage::~DirectoryManage()
{
    DeleteDir(rootDir_);
}

void DirectoryManage::login()
{
    std::string account, password;
    std::cout << "--------- SimuOS Login ---------\n";
    std::cout << "Login: ";
    std::cin >> account;
    std::cout << "Password: ";
    std::cin >> password;
    user_ = QueryUser(account, password);
    if (user_.state == 0) {
        std::cout << "Login failed! Please try again.\n";
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
    User FailUser = {"login_failed", 0, "login_failed", "login_failed", 0};
    for (int i = 0; i < static_cast<int>(sizeof(userList) / sizeof(User)); i++) {
        if (account == userList[i].account) {
            if (password == userList[i].password) {
                userList[i].state = 1;
                std::cout << "Login successful! Welcome " << userList[i].name << "\n";
                return userList[i];
            } else {
                std::cout << "Incorrect password!\n";
                return FailUser;
            }
        }
    }
    std::cout << "Account not found!\n";
    return FailUser;
}

void DirectoryManage::CreateRootDirectory()
{
    rootDir_ = new Directory();
    rootDir_->name = "root";
    rootDir_->canRead = 1;
    rootDir_->canWrite = 0;
    std::cout << "Root directory created.\n";
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
                std::cout << "Directory name already exists.\n";
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
    std::cout << "Directory '" << name << "' created successfully.\n";
}

void DirectoryManage::cdDir(std::string path)
{
    if (path == "..") {
        if (workDir_->parentDirectory)
            workDir_ = workDir_->parentDirectory;
        else
            std::cout << "Already at root directory.\n";
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
    std::cout << "Invalid path.\n";
}

void DirectoryManage::rd(std::string name)
{
    if (workDir_->firstChildDirectory == nullptr) {
        std::cout << "No subdirectories to delete.\n";
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
            std::cout << "Directory '" << name << "' deleted successfully.\n";
            return;
        }
        p = p->nextDirectory.get();
    }
    std::cout << "Directory '" << name << "' not found.\n";
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

void DirectoryManage::ListByName(const std::string& name)
{
    // Check if it's a file first
    if (workDir_->firstChildFile) {
        DirectoryEntry* p = workDir_->firstChildFile.get();
        while (p) {
            if (p->file.name == name) {
                dirFile(name);
                return;
            }
            p = p->nextFile.get();
        }
    }
    // Check if it's a directory
    if (workDir_->firstChildDirectory) {
        Directory* p = workDir_->firstChildDirectory.get();
        while (p) {
            if (p->name == name) {
                ListDir(p);
                return;
            }
            p = p->nextDirectory.get();
        }
    }
    std::cout << "ls: cannot access '" << name << "': No such file or directory\n";
}

void DirectoryManage::dir()
{
    ListAll();
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
            std::cout << "Contents of " << path_ << ": \n";
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
                std::cout << "Creation time: " << std::ctime(&ptr->file.create_time);
                std::cout << "File name: " << ptr->file.name << "\n";
                std::cout << "Size: " << ptr->file.size << "\n";
                std::cout << "Attributes: ";
                std::cout << ((ptr->file.canRead == user_.userRight ||
                               ptr->file.canRead == 1)
                              ? "Readable  "
                              : "Not readable  ");
                std::cout << ((ptr->file.canWrite == user_.userRight ||
                               ptr->file.canWrite == 1)
                              ? "Writable\n"
                              : "Not writable\n");
                return;
            }
            ptr = ptr->nextFile.get();
        }
    }
    std::cout << "File not found.\n";
}

void DirectoryManage::ListDir(Directory* p)
{
    if (p->canRead != user_.userRight && p->canRead != 1) {
        std::cout << "Permission denied: cannot read directory info.\n";
        return;
    }
    if (p->firstChildDirectory || p->firstChildFile) {
        GetPath(p);
        std::cout << "Contents of " << path_ << ": \n";
    } else {
        std::cout << "Directory is empty.\n";
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
    std::cout << "Invalid path or directory not found.\n";
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
    std::cout << "File or directory '" << name << "' not found.\n";
}

void DirectoryManage::RenameDir(Directory* ptr, std::string newName)
{
    if (ptr->canWrite != 1 && ptr->canWrite != user_.userRight) {
        std::cout << "Permission denied: cannot rename.\n";
        return;
    }
    Directory* p = workDir_->firstChildDirectory.get();
    while (p) {
        if (p != ptr && p->name == newName) {
            std::cout << "Directory name already exists, cannot rename.\n";
            return;
        }
        p = p->nextDirectory.get();
    }
    ptr->name = newName;
    std::cout << "Directory renamed to '" << ptr->name << "'.\n";
}

void DirectoryManage::RenameFile(DirectoryEntry* ptr, std::string newName)
{
    if (ptr->file.canWrite != 1 && ptr->file.canWrite != user_.userRight) {
        std::cout << "Permission denied: cannot rename.\n";
        return;
    }
    DirectoryEntry* p = workDir_->firstChildFile.get();
    while (p) {
        if (p != ptr && p->file.name == newName) {
            std::cout << "File name already exists, cannot rename.\n";
            return;
        }
        p = p->nextFile.get();
    }
    ptr->file.name = newName;
    std::cout << "File renamed to '" << ptr->file.name << "'.\n";
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
            std::cout << "File name already exists. Please use a different name.\n";
            return File{};
        }
        p = p->nextFile.get();
    }
    File newFile;
    newFile.name = name;
    std::cout << "Enter file content:\n";
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
                    std::cout << "Permission denied: cannot write to file.\n";
                    return nullptr;
                }
                std::cout << "Enter file content:\n";
                std::string context;
                std::cin.ignore();
                std::getline(std::cin, context);
                p->file.context += context;
                std::cout << "File written successfully.\n";
                return &p->file;
            }
            p = p->nextFile.get();
        }
    }
    std::cout << "File '" << name << "' not found.\n";
    return nullptr;
}

DirectoryManage::File* DirectoryManage::OpenFile(std::string name)
{
    if (workDir_->firstChildFile) {
        DirectoryEntry* p = workDir_->firstChildFile.get();
        while (p) {
            if (p->file.name == name) {
                std::cout << "File opened successfully.\n";
                return &p->file;
            }
            p = p->nextFile.get();
        }
    }
    std::cout << "File '" << name << "' not found.\n";
    return nullptr;
}

DirectoryManage::File* DirectoryManage::CloseFile(std::string name)
{
    if (workDir_->firstChildFile) {
        DirectoryEntry* p = workDir_->firstChildFile.get();
        while (p) {
            if (p->file.name == name) {
                std::cout << "File closed successfully.\n";
                return &p->file;
            }
            p = p->nextFile.get();
        }
    }
    std::cout << "File '" << name << "' not found.\n";
    return nullptr;
}
