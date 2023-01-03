launch:threadManage.o directoryManage.o launch.o diskManage.o memoryManage.o
	g++ -std=c++11 *.o -o launch
	./launch
	rm *.o
	rm launch
threadManage.o:threadManage.cpp
	g++ -std=c++11 -c threadManage.cpp -o threadManage.o
directoryManage.o:directoryManage.cpp
	g++ -std=c++11 -c directoryManage.cpp -o directoryManage.o
launch.o:launch.cpp
	g++ -std=c++11 -c launch.cpp -o launch.o
diskManage.o:diskManage.cpp
	g++ -c -std=c++11 diskManage.cpp -o diskManage.o
memoryManage.o:memoryManage.cpp
	g++ -c -std=c++11 memoryManage.cpp -o memoryManage.o
clean:
	rm *.o
	rm launch
