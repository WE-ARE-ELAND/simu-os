CXX       = C:/Users/Elon/scoop/apps/mingw/current/bin/g++
CXXFLAGS  = -std=c++11 -MMD -MP -Wall -Wextra -Iinclude -Isrc -pthread -fexec-charset=gbk -finput-charset=utf-8
EXEEXT    =

OBJS = src/threadManage.o \
       src/cli.o \
       src/directoryManage.o \
       src/launch.o \
       src/diskManage.o \
       src/memoryManage.o

TEST_OBJS = $(OBJS)
TESTS = build/test_disk build/test_directory build/test_memory

TARGET = build/launch$(EXEEXT)

.PHONY: all clean run tests

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p build 2>/dev/null || true
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

tests: $(TESTS)

build/test_disk: src/diskManage.o tests/test_disk.cpp
	@mkdir -p build 2>/dev/null || true
	$(CXX) $(CXXFLAGS) tests/test_disk.cpp src/diskManage.o -o $@

build/test_directory: src/directoryManage.o tests/test_directory.cpp
	@mkdir -p build 2>/dev/null || true
	$(CXX) $(CXXFLAGS) tests/test_directory.cpp src/directoryManage.o -o $@

build/test_memory: src/memoryManage.o tests/test_memory.cpp
	@mkdir -p build 2>/dev/null || true
	$(CXX) $(CXXFLAGS) tests/test_memory.cpp src/memoryManage.o -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	@./$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)
	$(RM) src/*.d include/*.d
	$(RM) $(TESTS)

-include $(OBJS:.o=.d)
