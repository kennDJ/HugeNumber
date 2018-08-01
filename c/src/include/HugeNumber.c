#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<stdlib.h>

#include"HugeNumber.h"

int indexOf(HUGENUMBER myNumber, int current_Index) {
	return (current_Index >= myNumber.number_count) ? MIN_BIT : myNumber.number_array[current_Index];
}

int getMecCode(int value, char number_sign) {
	return (number_sign == NEGATIVE_SIGN) ? (MAX_BIT - value) : value;
}

HUGENUMBER addHugeNumber(HUGENUMBER myNumber1, HUGENUMBER myNumber2) {
	int i;
	int carry_data;
	HUGENUMBER addResult = {0};

	addResult.number_count = ((myNumber1.number_count > myNumber2.number_count)? myNumber1.number_count : myNumber2.number_count) + 1; 					//以最长的巨大数数组长度+1作为总循环次数
	addResult.number_array = (int *)calloc(sizeof(int), addResult.number_count); 	//永远多申请一块数组空间，也有可能这块空间会被浪费

	for(i = 0, carry_data = 0; i < addResult.number_count; i++) {
		int eachResult = getMecCode(indexOf(myNumber1, i), myNumber1.number_sign) + getMecCode(indexOf(myNumber2, i), myNumber2.number_sign);
		addResult.number_array[i] = (eachResult + carry_data) % CURRENT_SYSTEM;
		carry_data = (eachResult + carry_data) / CURRENT_SYSTEM;					//考虑连续进位的情况存在..
	}
	addResult.number_sign = myNumber1.number_sign ^ myNumber2.number_sign ^ carry_data;

	for(i = 0; i < addResult.number_count; i++) {
		int value = addResult.number_array[i] + carry_data;

		addResult.number_array[i] = getMecCode(value % CURRENT_SYSTEM, addResult.number_sign);
		carry_data = value / CURRENT_SYSTEM;
	}

	return eliminateZeros(&addResult);
}

HUGENUMBER subHugeNumber(HUGENUMBER myNumber1, HUGENUMBER myNumber2) {
	HUGENUMBER subResult = {0};

	myNumber2.number_sign = !myNumber2.number_sign;
	subResult = addHugeNumber(myNumber1, myNumber2);

	return eliminateZeros(&subResult);
}

HUGENUMBER mulHugeNumber(HUGENUMBER myNumber1, HUGENUMBER myNumber2) {
	int i;
	int j;
	HUGENUMBER mulResult = {0};

	mulResult.number_count = myNumber1.number_count + myNumber2.number_count;
	mulResult.number_array = (int *)calloc(sizeof(int), mulResult.number_count);
	mulResult.number_sign = myNumber1.number_sign ^ myNumber2.number_sign;    //1^0=1,1^1=0,0^0=0

	for(i = 0; i < myNumber2.number_count; i++) {
		int carry_data = 0;
		int result_index = i;
		for(j = 0; j < myNumber1.number_count; j++) {
			int eachResult = myNumber2.number_array[i] * myNumber1.number_array[j] + carry_data;
			carry_data = (mulResult.number_array[result_index] + eachResult) / CURRENT_SYSTEM;
			mulResult.number_array[result_index] = (mulResult.number_array[result_index] + eachResult) % CURRENT_SYSTEM;
			result_index++;
		}
		mulResult.number_array[result_index] = carry_data;
	}

	return eliminateZeros(&mulResult);
}

//巨大数除法 提供两个算法可完成除法   迭代相减divThroughSub   二分搜索BinarySearchMethod 均可完成除法
HUGENUMBER divHugeNumber(HUGENUMBER dividendHug, HUGENUMBER divisorHug) {
	char sign;
	HUGENUMBER divResult = {0};													//除法运算的商

	sign = divisorHug.number_sign ^ divisorHug.number_sign;
	divisorHug.number_sign = dividendHug.number_sign = POSTIVE_SIGN;
//如果被除数小于除数 直接返回商为0 余数就是被除数
	if(CompareHugeNumber(dividendHug, divisorHug) == LESS_THAN) {
		divResult.number_count = 1;
		divResult.number_array = (int *)calloc(sizeof(int), 1);
		return divResult;
	}

	// divResult = BinarySearchMethod(dividendHug, divisorHug);
	divResult = iterationSubtracting(dividendHug, divisorHug);
	divResult.number_sign = sign;

	return eliminateZeros(&divResult);
}

//根据已完成的除法完成取余函数
HUGENUMBER modHugeNumber(HUGENUMBER dividendHug, HUGENUMBER divisorHug) {
	char sign;
	HUGENUMBER modResult = {0};
	HUGENUMBER biggerDivsorHug = {0};											//用来保存放大后的除数
	HUGENUMBER amplification = {0};

	sign = divisorHug.number_sign ^ divisorHug.number_sign;
	divisorHug.number_sign = dividendHug.number_sign = POSTIVE_SIGN;

	if(CompareHugeNumber(dividendHug, divisorHug) == LESS_THAN) { //如果出现a % b , a < b,直接返回a
		modResult = dividendHug;
		modResult.number_sign = sign;

		return modResult;
	}

	while(CompareHugeNumber(divisorHug, dividendHug) == LESS_THAN) {
		amplification = getMinDivResult(dividendHug, divisorHug);  				//得到倍数
		biggerDivsorHug = mulHugeNumber(amplification, divisorHug);			//左移除数或者用乘法放大除数，明显乘法放大要快一点
		while(CompareHugeNumber(biggerDivsorHug, dividendHug) == LESS_THAN) {
			dividendHug = subHugeNumber(dividendHug, biggerDivsorHug);			//开始被除数减去除数的减法运算
		}
		// printf("each:");
		// showHugeInfor(dividendHug);
	}

	modResult = dividendHug;
	modResult.number_sign = sign;

	destoryHugnumber(&biggerDivsorHug);
	destoryHugnumber(&amplification);

	return eliminateZeros(&modResult);
}

//判断两个巨大数绝大小 为除法运算中何时停止减法服务
//hugenumber1 < hugenumber2 返回LESS_THAN
//hugenumber1 >= hugenumber2，返回NOT_LESS_THAN
int CompareHugeNumber(HUGENUMBER hugenumber1, HUGENUMBER hugenumber2) {
//在进行除法的时候一律改变成正数相除 判断大小直接让两数相减
	return subHugeNumber(hugenumber1, hugenumber2).number_sign == NEGATIVE_SIGN ? LESS_THAN : NOT_LESS_THAN;
}


boolean readFileInfor(HUGENUMBER *myNumber, const char *fileName) {
	FILE *fp;

	fp = fopen(fileName, "r");
	if(fp == NULL) {
		printf("File is not exist!");
		return FALSE;
	}

	initHugeNumber(myNumber, fp);
	return TRUE;
}

//The read file operation in this section is only for integer operations,
// not counting the decimal, and needs to be modified.
void initHugeNumber(HUGENUMBER *myNumber, FILE *fp) {
	int file_length;
	int beginIndex;
	int i;
	char firstSign;
	const char *format[4] = {"%4d", "%1d", "%2d", "%3d"};

	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&firstSign, sizeof(char), 1, fp);							//先读一个字符判断是不是数字，决定巨大数实际起始位置
	beginIndex = ('0' <= firstSign && firstSign <= '9') ? 0 : 1;    //文件中巨大数实际实际起始位置
	file_length += ('0' <= firstSign && firstSign <= '9') ? 0 : -1; //文件中巨大数实际长度 以此来计算申请数组长度
	fseek(fp, beginIndex, SEEK_SET);
	myNumber->number_sign = (firstSign == '-') ? NEGATIVE_SIGN : POSTIVE_SIGN;
	myNumber->number_count = (file_length + 3) / 4;
	myNumber->number_array = (int *)calloc(sizeof(int), (file_length + 3) / 4);

	fscanf(fp, format[file_length % 4], &myNumber->number_array[myNumber->number_count - 1]);
	for(i = myNumber->number_count - 2; i >= 0; i--) {
		fscanf(fp, "%4d", &myNumber->number_array[i]);
	}

	fclose(fp);
}



//为getMinDivResult()函数服务，从巨大数高位开始逐个判断数字大小，
//1 如果除数的第一个最高位数字大于被除数最高位第一个数字，直接返回FASLE
//2 如果除数的第一个最高位数字小于被除数最高位第一个数字，直接返回TRUE
//如果除数的第一个最高位数字等于被除数最高位第一个数字最高位数字继续重复1 2
boolean compareHighestNum(HUGENUMBER dividendHug, HUGENUMBER divisorHug) {
	int num1_length;
	int num2_length;
	int judgeTimes;
	int divisor_num;
	int dividend_num;

	num1_length = getHugenumerLength(dividendHug);
	num2_length = getHugenumerLength(divisorHug);
	judgeTimes = num1_length > num2_length ? num2_length : num1_length;

	while(judgeTimes > 0) {
		dividend_num = getHighestNum(dividendHug.number_array, num1_length);
		divisor_num = getHighestNum(divisorHug.number_array, num2_length);
		if(divisor_num > dividend_num) {
//如果除数最高位数字大于被除数最高位数字 直接返回FASLE
			return FALSE;
		}else if(divisor_num < dividend_num) {
//除数的第一个最高位数字小于被除数最高位第一个数字，直接返回TRUE
			return TRUE;
		}else {
//除数的第一个最高位数字等于被除数最高位第一个数字最高位数字继续重复上述判断
			num1_length--;
			num2_length--;
		}
		judgeTimes--;
	}

	return TRUE;
}


double k_pow(double base, int exponent) {
    if(exponent >= 0) {
		double ans = 1.0;
		while(exponent) {
			// 判断是偶数还是奇数 和1进行与运算
			if(exponent & 1) {
				// 如果此时的指数 为1则为奇数
				ans = ans * base;
			}
			base = base *base;         //底数进行乘方
			exponent = exponent >> 1;  //指数减半
		}

		return ans;
	}else {
		return 1.0 / k_pow(base, -1 * exponent); //指数为负数的情况 结果是一个分数
	}
}

//每次返回巨大数最高位的一个数字如21 1526 3545，第一次返回2第二次返回1
int getHighestNum(int *array, int hugeNumber_length) {

	if(hugeNumber_length % 4 == 0) {
		return array[(hugeNumber_length + 3) / 4 - 1] / THOUSAND;
	}else {
		int Denom = (int)k_pow(10.0, (double)((hugeNumber_length % 4) - 1));

		return (array[(hugeNumber_length + 3) / 4- 1] / Denom) % 10;
	}
}
//

// //每次返回巨大数最高位的一个数字如21 1526 3545，第一次返回2第二次返回1
// int getHighestNum(int *array, int hugeNumber_length) {
//
// 	return hugeNumber_length % 4 == 0 ? (array[((hugeNumber_length + 3) / 4) - 1] /
// 	THOUSAND) : (array[((hugeNumber_length + 3) / 4) - 1] / (int)pow(10.0, (double)((hugeNumber_length % 4) - 1))) % 10;
// }
//此函数作用得到初始商最小 例如23 4585 4396 / 12 4562   初始最小商就是1 0000
HUGENUMBER getMinDivResult(HUGENUMBER dividendHug, HUGENUMBER divisorHug) {
	HUGENUMBER Amplification = {0};
	int i;
	int Difference_length;

//通过调用compareHighestNum()函数来比较两个巨大数最高位的大小决定应该给除数补几个0
	Difference_length = compareHighestNum(dividendHug, divisorHug) ?
	 (getHugenumerLength(dividendHug) - getHugenumerLength(divisorHug)) :
	 (getHugenumerLength(dividendHug) - getHugenumerLength(divisorHug) - 1);
	Amplification.number_sign = POSTIVE_SIGN;
	Amplification.number_count = (Difference_length + 4) / 4;
	Amplification.number_array = (int *)calloc(sizeof(int), Amplification.number_count);

	for(i = 0; i < Amplification.number_count - 1; i++) {
		Amplification.number_array[i] = 0;					//倍数的数据除去最高位都是0
	}

	const int formatZeros[] = {1,10,100,1000};

//还是需要通过两巨大数长度之差，例如之差为5时，目标倍数为0010 0000，则得到10的过程就是pow(10, 5 % 4)
// 注：在gcc下double转为int类型 会出现100变为99的情况
	// Amplification.number_array[Amplification.number_count - 1] = (int)pow(10.0, (double)(Difference_length % 4));
	Amplification.number_array[Amplification.number_count - 1] = formatZeros[Difference_length % 4];

	return Amplification;
}


//通过减法完成的除法
HUGENUMBER iterationSubtracting(HUGENUMBER dividendHug, HUGENUMBER divisorHug) {
	HUGENUMBER divResult = {0};													//除法运算的商
	HUGENUMBER biggerDivsorHug = {0};											//用来保存放大后的除数
	HUGENUMBER amplification = {0};

	while(CompareHugeNumber(divisorHug, dividendHug) == LESS_THAN) {
		amplification = getMinDivResult(dividendHug, divisorHug);  				//得到倍数
		biggerDivsorHug = mulHugeNumber(amplification, divisorHug);			//左移除数或者用乘法放大除数，明显乘法放大要快一点
		while(CompareHugeNumber(biggerDivsorHug, dividendHug) == LESS_THAN) {
			divResult = addHugeNumber(divResult, amplification);				//保存商
			dividendHug = subHugeNumber(dividendHug, biggerDivsorHug);			//开始被除数减去除数的减法运算
		}
		// 这里有内存泄漏问题 所以还要加上销毁函数 在循环里面
		destoryHugnumber(&biggerDivsorHug);
		destoryHugnumber(&amplification);
	}

	return eliminateZeros(&divResult);
}



//去掉巨大数中高位的无效0数字，0000 0000 0023 5985 = 0023 5985
//思路：先调用getHugenumerLength(HUGENUMBER myNumber)方法得到巨大数有效数字长度
//根据长度申请新空间，赋值，销毁旧巨大数，返回newHugenumber。

//这个函数方法还有问题 对于处理120+ -120 返回的结果巨大数长度 应该是0
HUGENUMBER eliminateZeros(HUGENUMBER *myNumber) {
	int actual_length = 0;
	HUGENUMBER newHugenumber = {0};
	int i;

	actual_length = getHugenumerLength(*myNumber);
	newHugenumber.number_sign = myNumber->number_sign;
	newHugenumber.number_count = (actual_length + 3) / 4;
	newHugenumber.number_array = (int *)calloc(sizeof(int), newHugenumber.number_count);

	for(i = 0; i < newHugenumber.number_count; i++) {
		newHugenumber.number_array[i] = myNumber->number_array[i];
	}

	destoryHugnumber(myNumber);

	return newHugenumber;
}
//得到一个巨大数在十进制下的位数实际长度 不包含最高位为0的
//例如0000 02354 9563 4182实际长度为11
//为函数getAmplification(HUGENUMBER dividendHug, HUGENUMBER divisorHug)服务
//为函数eliminateZeros(HUGENUMBER *myNumber)服务
int getHugenumerLength(HUGENUMBER myNumber) {
	const int array[] = {0, 10, 100, 1000, 10000};
	int i;
	int highestNum_length = 0;
//从巨大数高位开始 知道遇到有效不为0的数字停止
	for(myNumber.number_count = myNumber.number_count - 1; myNumber.number_array[myNumber.number_count] == 0; myNumber.number_count--);

	for(i = 0; i < 4; i++) {
		if(myNumber.number_array[myNumber.number_count] >= array[i] && myNumber.number_array[myNumber.number_count] < array[i+1]) {
			highestNum_length = i+1;			//记录巨大数最高位数字的位数
		}
	}

	return highestNum_length + myNumber.number_count * 4;
}

void destoryHugnumber(HUGENUMBER *myNumber) {
	if(myNumber->number_array == NULL) {
		return;
	}

	free(myNumber->number_array);
	myNumber->number_array = NULL;
}

HUGENUMBER getHighRange(HUGENUMBER low_range) {
	HUGENUMBER numberTen = {0};
	HUGENUMBER high_range = {0};

	numberTen.number_count = 1;
	numberTen.number_array = (int *)calloc(sizeof(int), numberTen.number_count);
	numberTen.number_array[0] = 10;								//给numberTen赋值为10  10*下限得到搜索曲线上限

	high_range = mulHugeNumber(low_range, numberTen);
	destoryHugnumber(&numberTen);

	return high_range;
}

//二分查找得到区间的中点
HUGENUMBER getBinaryMid(HUGENUMBER lowerNumber, HUGENUMBER highNumber) {
	HUGENUMBER result = {0};
	int i;
	int carry_data;

	result = addHugeNumber(lowerNumber, highNumber);
	for(i = result.number_count - 1, carry_data = 0; i >= 0; i--) {
		int eachResult = result.number_array[i] + carry_data * 10000;
		result.number_array[i] = eachResult / 2;
		carry_data = eachResult % 2;
	}

	return result;
}

//二分搜索算法
//过在初始区间上进行二分搜索 根据每次区间中点更新搜索区间 逼近最终商
////每次取得的区间中点和除数相乘，再让被除数减去试商 * 除数得到差值
//搜索迭代条件为：差值小于0或者 差值绝对值大于除数绝对值
//直到差值绝对值小于除数并且差值大于0， 停止搜索
HUGENUMBER BinarySearchMethod(HUGENUMBER dividendHug, HUGENUMBER divisorHug) {
	HUGENUMBER divResult = {0};													//搜索最终结果
	HUGENUMBER low_range = {0};													//搜索区间的下限
	HUGENUMBER high_range = {0};												//搜索区间的上限
	HUGENUMBER differenceWithDivdend = {0};

	low_range = getMinDivResult(dividendHug, divisorHug);   									//得到搜索商区间的初始下限
	high_range = getHighRange(low_range);														//得到搜索区间的上限
	divResult = getBinaryMid(high_range, low_range);
	differenceWithDivdend = subHugeNumber(dividendHug, mulHugeNumber(divisorHug, divResult));	//计算差值
	//开始二分搜索查找商
	while((CompareHugeNumber(divisorHug, differenceWithDivdend) == LESS_THAN) || (differenceWithDivdend.number_sign == NEGATIVE_SIGN)) {
		if(differenceWithDivdend.number_sign == NEGATIVE_SIGN) {
			high_range = divResult;													//此处表示当前商过于大了 应该更新区间上限为小的方向
		}else {
			if(CompareHugeNumber(differenceWithDivdend, divisorHug) == LESS_THAN) { //此处差值的值为正 判断值是否小于除数
				break;																//如果差值小于除数，则搜索停止 已经找到商 余数就是此时的差值
			}else {
				low_range = divResult;												//如果差值不小于除数，说明当前试商过于小，更新区间下限为大的方向
			}
		}
		divResult = getBinaryMid(high_range, low_range);
		differenceWithDivdend = subHugeNumber(dividendHug, mulHugeNumber(divisorHug, divResult));
	}

	destoryHugnumber(&low_range);
	destoryHugnumber(&high_range);
	destoryHugnumber(&differenceWithDivdend);

	return divResult;
}

// //通过移动达到放大除数的目的，避免乘法放大，发现速度并没有乘法放大快
// //最快的速度还是通过除法放大最快
// //由此可见此方法很鸡肋
// HUGENUMBER removeHugenumber(HUGENUMBER hugenumber1, HUGENUMBER hugenumber2) {
// 	HUGENUMBER result = {0};
// 	int Difference_length;
// 	int i;
// 	int offset1;
// 	int offset2;

// 	Difference_length = compareHighestNum(hugenumber1, hugenumber2) ? (getHugenumerLength(hugenumber1) - getHugenumerLength(hugenumber2)) : (getHugenumerLength(hugenumber1) - getHugenumerLength(hugenumber2) - 1);
// 	result.number_sign = hugenumber2.number_sign;
// 	result.number_count = hugenumber1.number_count;
// 	result.number_array = (int *)calloc(sizeof(int), result.number_count);
// 	offset1 = (int)pow(10.0, (double)(Difference_length % 4));
// 	offset2 =  10000 / offset1;
// 	for(i = 0; i < hugenumber2.number_count; i++) {
// 		int num = (hugenumber2.number_array[i] % offset2) * offset1;
// 		result.number_array[i + (Difference_length / 4)] = i == 0 ? num : (num + hugenumber2.number_array[i - 1] / offset2);
// 	}
// 	result.number_array[result.number_count - 1]  = hugenumber2.number_array[hugenumber2.number_count - 1] / offset2;

// 	return result;
// }
