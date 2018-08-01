#ifndef _HUGE_NUMBER_H_
#define _HUGE_NUMBER_H_

// #ifndef _HEAD_FILE_
// #define _HEAD_FILE_
// #include<stdio.h>
// #include<malloc.h>
// #include<math.h>
// #include<stdlib.h>
// #endif

#define POSTIVE_SIGN		0
#define NEGATIVE_SIGN		1

#define LESS_THAN			1
#define NOT_LESS_THAN		0

#define TRUE				1
#define FALSE				0
#define ERROR_FLAG			-1

#define ONE					1
#define ZEROS 				0
#define THOUSAND			1000

#define MAX_BIT				9999
#define MIN_BIT				0
#define CURRENT_SYSTEM		10000

typedef unsigned char boolean;

typedef struct HUGENUMBER {
	char number_sign;			//标记正负符号
	int *number_array;			//指向存储巨大数的数组
	int number_count;			//数组中元素个数
	short number_power;
}HUGENUMBER;

//函数fscanf原型int fscanf(FILE*stream, const char *format, [argument...])
//判断巨大数实际长度和4取余结果，用取余结果当做下标在format数组里取得字符串做fscanf格式符
//取余结果 0 1 2 3
//读取长度 4 1 2 3
boolean readFileInfor(HUGENUMBER *myNumber, const char *fileName);

//根据传递进来的文件指针，将文件数据存储到结构体中
void initHugeNumber(HUGENUMBER *myNumber, FILE *fp);

//销毁巨大数
void destoryHugnumber(HUGENUMBER *myNumber);

//以a+b = c的形式将两个整数巨大数相加结果返回，需注意的是要释放结果中的数组空间
HUGENUMBER addHugeNumber(HUGENUMBER myNumber1, HUGENUMBER myNumber2);

//两个巨大数相减等于 第一个数加第二个数的相反数
HUGENUMBER subHugeNumber(HUGENUMBER myNumber1, HUGENUMBER myNumber2);

//乘法运算申请的结果数组长度为乘数与被乘数长度之和
HUGENUMBER mulHugeNumber(HUGENUMBER myNumber1, HUGENUMBER myNumber2);

//根据被除数去减除数的原理实现巨大数除法
HUGENUMBER divHugeNumber(HUGENUMBER dividendHug, HUGENUMBER divisorHug);

//根据已完成的除法完成取余函数
HUGENUMBER modHugeNumber(HUGENUMBER dividendHug, HUGENUMBER divisorHug);


//微易码补码主要处理负数如何存储到万进制数组方便后续的减法运算，实际上是正数+负数
//负数的补码是9999-数值，正数补码就是正数本身。
//方便的是 调用一次 原码转为补码 调用第二次 补码转为原码
int getMecCode(int value, char number_sign);

//在加法运算中，以最长的巨大数当做循环标准
//判断当前循环次数是否超过数组长度，超过返回0，未超过则返回对应下标数组值
int indexOf(HUGENUMBER myNumber, int current_Index);

//判断两个巨大数绝对值的大小
//hugenumber1 < hugenumber2返回LESS_THAN,
//hugenumber1 >= hugenumber2 返回NOT_LESS_THAN
int CompareHugeNumber(HUGENUMBER hugenumber1, HUGENUMBER hugenumber2);

//得到巨大数的实际长度 不包含高位0数字长度
int getHugenumerLength(HUGENUMBER myNumber);

//通过计算得到除数乘的最大倍数 10的某次方
HUGENUMBER getMinDivResult(HUGENUMBER dividendHug, HUGENUMBER divisorHug);

//得到二分搜索区间中点
HUGENUMBER getBinaryMid(HUGENUMBER lowerNumber, HUGENUMBER highNumber);

//二分搜索
HUGENUMBER BinarySearchMethod(HUGENUMBER dividendHug, HUGENUMBER divisorHug);

//二分搜索得到搜索区间上限
HUGENUMBER getHighRange(HUGENUMBER low_range);

//去除巨大数高位无效数据0的方法
HUGENUMBER eliminateZeros(HUGENUMBER *myNumber);

//比较巨大数最高位的一个数字大小
boolean compareHighestNum(HUGENUMBER dividendHug, HUGENUMBER divisorHug);

//通过减法完成的除法
HUGENUMBER iterationSubtracting(HUGENUMBER dividendHug, HUGENUMBER divisorHug);

//得到巨大数高位上的一个数字 主要在在除法中放大除数 需要的东西
int getHighestNum(int *array, int hugeNumber_length);

void showHugeInfor(HUGENUMBER myNumber);

// linux下的pow函数有点问题 需要自己写一个
double k_pow(double base, int exponent);

#endif
