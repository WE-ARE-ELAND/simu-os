threadManage_test:threadManage.o directoryManage.o threadManage_test.o
	g++ -std=c++11 *.o -o threadManage_test
	./threadManage_test
	rm *.o
	rm threadManage_test
threadManage.o:threadManage.cpp
	g++ -std=c++11 -c threadManage.cpp -o threadManage.o
directoryManage.o:directoryManage.cpp
	g++ -std=c++11 -c directoryManage.cpp -o directoryManage.o
threadManage_test.o:threadManage_test.cpp
	g++ -std=c++11 -c threadManage_test.cpp -o threadManage_test.o
clean:
	rm *.o
	rm threadManage_test
