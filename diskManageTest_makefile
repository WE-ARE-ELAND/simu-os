diskManageTest:diskManageTest.o diskManage.o
	g++ *.o -o diskManageTest
	./diskManageTest
	rm *.o
	rm ./diskManageTest
diskManageTest.o:diskManageTest.cpp
	g++ -c -std=c++11 diskManageTest.cpp -o diskManageTest.o
diskManage.o:diskManage.cpp
	g++ -c -std=c++11 diskManage.cpp -o diskManage.o