gcc -c ./include/HugeNumber.c
gcc ./include/HugeNumber.c test.c -o run
rm -rf *.o
./run
date >> ./lib/output.txt
./run >> ./lib/output.txt
