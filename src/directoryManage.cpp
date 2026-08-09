#include "directoryManage.h"

DirectoryManage::DirectoryManage()
    : workDir_(nullptr), rootDir_(nullptr)
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
    while (user_.state == 0)
    {
        std::string account, password;
        std::cout << "--------- SimuOS Login ---------\n";
        std::cout << "Login: ";
        std::cin >> account;
        std::cout << "Password: ";
        std::cin >> password;
        user_ = QueryUser(account, password);
        if (user_.state == 0)
            std::cout << "Login failed! Please try again.\n\n";
    }
    CreateRootDirectory();
    MakeDir(user_.name);
}

DirectoryManage::User DirectoryManage::QueryUser(const std::string& account, const std::string& password)
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

void DirectoryManage::tree(Directory* curDir, int level) const
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

void DirectoryManage::MakeDir(const std::string& name)
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

void DirectoryManage::cdDir(const std::string& path)
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

void DirectoryManage::rd(const std::string& name)
{
    if (workDir_->firstChildDirectory == nullptr) {
        std::cout << "No subdirectories to delete.\n";
        return;
    }
    Directory* p = workDir_->firstChildDirectory.get();
    while (p) {
        if (p->name == name) {
            Directory* prev = p->frontDirectory;
            std::unique_ptr<Directory> cur;
            if (prev)
                cur = std::move(prev->nextDirectory);
            else
                cur = std::move(workDir_->firstChildDirectory);
            Directory* tail = cur->nextDirectory.release();
            if (prev) {
                prev->nextDirectory.reset(tail);
                if (tail) tail->frontDirectory = prev;
            } else {
                workDir_->firstChildDirectory.reset(tail);
                if (tail) tail->frontDirectory = nullptr;
            }
            Directory* doomed = cur.release();
            DeleteDir(doomed);
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
        Directory* ptr = p->firstChildDirectory.release();
        while (ptr) {
            Directory* next = ptr->nextDirectory.release();
            DeleteDir(ptr);
            ptr = next;
        }
    }
    if (p->firstChildFile) {
        DirectoryEntry* ptr = p->firstChildFile.release();
        while (ptr) {
            DirectoryEntry* next = ptr->nextFile.release();
            delete ptr;
            ptr = next;
        }
    }
    delete p;
}

void DirectoryManage::ListByName(const std::string& name) const
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

void DirectoryManage::ListAll() const
{
    std::cout << "----------------------------------------------------------------\n";
    std::queue<Directory*> q;
    q.push(workDir_);
    while (!q.empty()) {
        Directory* p = q.front();
        q.pop();
        if (p->firstChildDirectory || p->firstChildFile) {
            std::cout << "Contents of " << GetPath(p) << ": \n";
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

std::vector<std::string> DirectoryManage::listChildNames() const
{
    std::vector<std::string> names;
    if (workDir_->firstChildDirectory) {
        for (Directory* p = workDir_->firstChildDirectory.get(); p; p = p->nextDirectory.get())
            names.push_back(p->name);
    }
    if (workDir_->firstChildFile) {
        for (DirectoryEntry* p = workDir_->firstChildFile.get(); p; p = p->nextFile.get())
            names.push_back(p->file.name);
    }
    return names;
}

void DirectoryManage::dirFile(const std::string& name) const
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

void DirectoryManage::ListDir(Directory* p) const
{
    if (p->canRead != user_.userRight && p->canRead != 1) {
        std::cout << "Permission denied: cannot read directory info.\n";
        return;
    }
    if (p->firstChildDirectory || p->firstChildFile) {
        std::cout << "Contents of " << GetPath(p) << ": \n";
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

void DirectoryManage::dirPath(const std::string& path) const
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

void DirectoryManage::ren(const std::string& name, const std::string& newName)
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

void DirectoryManage::RenameDir(Directory* ptr, const std::string& newName)
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

void DirectoryManage::RenameFile(DirectoryEntry* ptr, const std::string& newName)
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

std::string DirectoryManage::GetPath(Directory* curDir) const
{
    std::string path;
    std::stack<Directory*> q;
    Directory* d = curDir;
    while (d) { q.push(d); d = d->parentDirectory; }
    while (!q.empty()) {
        path += q.top()->name;
        q.pop();
        if (!q.empty()) path += "\\";
    }
    return path;
}

DirectoryManage::File DirectoryManage::CreateFile(const std::string& name)
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

bool DirectoryManage::DeleteFile(const std::string& name)
{
    if (workDir_->firstChildFile == nullptr)
        return false;
    DirectoryEntry* ptr = workDir_->firstChildFile.get();
    while (ptr) {
        if (ptr->file.name == name) {
            DirectoryEntry* prev = ptr->frontFile;
            std::unique_ptr<DirectoryEntry> cur;
            if (prev)
                cur = std::move(prev->nextFile);
            else
                cur = std::move(workDir_->firstChildFile);
            DirectoryEntry* tail = cur->nextFile.release();
            if (prev) {
                prev->nextFile.reset(tail);
                if (tail) tail->frontFile = prev;
            } else {
                workDir_->firstChildFile.reset(tail);
                if (tail) tail->frontFile = nullptr;
            }
            if (ptr == workDir_->lastChildFile)
                workDir_->lastChildFile = prev;
            delete cur.release();
            return true;
        }
        ptr = ptr->nextFile.get();
    }
    return false;
}

DirectoryManage::File* DirectoryManage::ReadFile(std::string path) const
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
            if (p->firstChildFile) {
                DirectoryEntry* t = p->firstChildFile.get();
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

DirectoryManage::File* DirectoryManage::WriteFile(const std::string& name)
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

DirectoryManage::File* DirectoryManage::OpenFile(std::string name) const
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

DirectoryManage::File* DirectoryManage::CloseFile(std::string name) const
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

bool DirectoryManage::isDir(const std::string& name) const
{
    if (workDir_->firstChildDirectory) {
        for (Directory* p = workDir_->firstChildDirectory.get(); p; p = p->nextDirectory.get())
            if (p->name == name) return true;
    }
    return false;
}

bool DirectoryManage::isFile(const std::string& name) const
{
    if (workDir_->firstChildFile) {
        for (DirectoryEntry* p = workDir_->firstChildFile.get(); p; p = p->nextFile.get())
            if (p->file.name == name) return true;
    }
    return false;
}

const DirectoryManage::File* DirectoryManage::getFile(const std::string& name) const
{
    if (workDir_->firstChildFile) {
        for (DirectoryEntry* p = workDir_->firstChildFile.get(); p; p = p->nextFile.get())
            if (p->file.name == name) return &p->file;
    }
    return nullptr;
}
