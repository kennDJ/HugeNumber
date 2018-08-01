g++ -c include/HugeNumber.cpp
g++ HugeNumber.o Demo.cpp -o get
rm -rf HugeNumber.o
./get
date >> lib/output.txt
./get >> lib/output.txt
