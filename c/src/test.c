#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>

#include"./include/HugeNumber.h"

//巨大数高位先打印

void showHugeInfor(HUGENUMBER myNumber) {
	int i;

	printf("[%c]", (myNumber.number_sign == NEGATIVE_SIGN)? '-' :'+');
	if(myNumber.number_count == 0) {
		printf("0000\n");
	}
	for(i = myNumber.number_count - 1; i >= 0; i--) {
		printf("%04d ", myNumber.number_array[i]);
	}
	printf("\n");
}

int main() {
	const char *addNumFileName = "../file/Num1.txt";
	const char *augendNumFileName = "../file/Num2.txt";
	boolean readFileResult_1;
	boolean readFileResult_2;

	long before_time;
	long after_time;
	long total_Time;

	HUGENUMBER addNumber = {0};
	HUGENUMBER augendNumber = {0};
	HUGENUMBER result = {0};

	readFileResult_1 = readFileInfor(&addNumber, addNumFileName);
	readFileResult_2 = readFileInfor(&augendNumber, augendNumFileName);

	if(!readFileResult_1 || !readFileResult_2) {
		printf("ReadFile Error !\n");
		return 0;
	}



	showHugeInfor(addNumber);
	showHugeInfor(augendNumber);

	printf("length = %d\n", getHugenumerLength(addNumber));

before_time = clock();
	// result = mulHugeNumber(addNumber, augendNumber);
	// result = addHugeNumber(addNumber, augendNumber);
	// result = divHugeNumber(addNumber, augendNumber);
after_time = clock();
total_Time = after_time - before_time;

	// showHugeInfor(result);

printf("\n%d digits dividend by %d digits took", getHugenumerLength(addNumber),getHugenumerLength(augendNumber));
printf(" %ld.%03ld seconds\n", total_Time / 1000, total_Time % 1000);

	destoryHugnumber(&addNumber);
	destoryHugnumber(&augendNumber);
	destoryHugnumber(&result);

	return 0;
}
