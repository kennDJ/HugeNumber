#ifndef _HUGE_NUMBER_METHOD_AND_DATA_H_
#define _HUGE_NUMBER_METHOD_AND_DATA_H_

#define	NEGATIVE_SIGN				1
#define POSTIVE_SIGN				0
#define NO_SIGN				   		-1

#define ONE									1
#define ZEROS 							0
#define THOUSAND						1000

#define MAX_BIT							9999
#define MIN_BIT							0
#define CURRENT_SYSTEM			10000

/**
 * LESS_THAN小于的意思 2 <  3
 * NOT_LESS_THAN  包括大于和等于
 */
#define LESS_THAN				1
#define NOT_LESS_THAN			0

typedef struct NUMBERMODEL {
	int number_count; 		//记录数组的长度
	int *number_array;      //动态申请巨大数数组
	bool number_sign;		//表示巨大数的正负 0正 1负
}NUMBERMODEL;

class HUGENUMBER {
	// 封装的东西有很多
	// 包括一些方法 成员
private:

	//数据模型 为了方便写了一个结构体 也可以写成单独的三个类成员 需要更多的getandset方法
	// 对于结构体和类的理解 结构体的成员访问权限都是public的 而面向对象中类加入的权限修饰符
	NUMBERMODEL numberModel;

	/**
	*微易码补码主要处理负数如何存储到万进制数组方便后续的减法运算，实际上是正数+负数
	*负数的补码是9999-数值，正数补码就是正数本身。
	*方便的是 调用一次 原码转为补码 调用第二次 补码转为原码
	*/
	int getMecCode(int value, bool number_sign);

	//在加法运算中，以最长的巨大数当做循环标准
	//判断当前循环次数是否超过数组长度，超过返回0，未超过则返回对应下标数组值
	int indexOf(NUMBERMODEL myNumber, int current_Index);

// 初始化字符串 将字符串处理到数组中存储
	void initHugeNumber(string num);

// 判断字符是否为数字
	bool charIsNum(char ch);

// 去除掉结果开始多余的0 主要是因为乘法的原因会在高位产生一些无效数据
	void eliminateZeros(NUMBERMODEL &numberModel);

	//每次返回巨大数最高位的一个数字如21 1526 3545，第一次返回2第二次返回1
	int getHighestNum(int *array, int hugeNumber_length);

	int getCount(int len);

// 除法运算中的迭代相减
	void iterationSubtracting(HUGENUMBER val);

// 判断两个巨大数最高位的单独一个数字的大小 相同的情况下 顺延至更低位  直到比出来大小
	bool compareHighestNum(NUMBERMODEL dividendHug, NUMBERMODEL divisorHug);

// 在巨大数除法中 需要放大除数 然后用被除数减去除数 该函数的作用在于得到这个放大倍数
	HUGENUMBER* getMinDivResult(HUGENUMBER val);

// 得到一个巨大数实际的十进制长度 不包括符号 不包括高位无效0
	int getHugenumberLength(NUMBERMODEL numberModel);

// 封装一个销毁释放内存函数 减少代码的重写
	void KwenDelete(int* &array);

// 封装一个申请内存函数 附带对内存的初始化
	int* NewInt(int length);
public:

	// 默认构造函数
	HUGENUMBER();

// 以字符串的方式传递巨大数
	HUGENUMBER(string num);
	HUGENUMBER(char num[]);

	// 深拷贝方法
	HUGENUMBER(const HUGENUMBER &val);

	~HUGENUMBER();

	void SetHugeNumber(string num);

	void show();

	NUMBERMODEL getNumberModel();

	void setNumberModel(NUMBERMODEL model);

	//以a+b = c的形式将两个整数巨大数相加结果返回，需注意的是要释放结果中的数组空间
	HUGENUMBER* add(HUGENUMBER Addend);

	//两个巨大数相减等于 第一个数加第二个数的相反数
	HUGENUMBER* sub(HUGENUMBER Subtraction);

	//乘法运算申请的结果数组长度为乘数与被乘数长度之和
	HUGENUMBER* mul(HUGENUMBER MulNum);

	//根据被除数去减除数的原理实现巨大数除法
	HUGENUMBER* div(HUGENUMBER val);

	//根据已完成的除法完成取余函数
	HUGENUMBER* mod(HUGENUMBER val);

// 快速幂算法求幂次方
	HUGENUMBER* hug_pow(int exponent);

// 比较两个巨大数大小 都是正数的前提下
	bool compareTo(HUGENUMBER val);
};
#endif
