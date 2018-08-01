/*
*作者：张宇坤
*时间：2018/7/23
*开始完成代码的注释
*
*
*
*
*
*
*
*
*
*
*/

#ifndef _SOME_HEADFILE_
#define _SOME_HEADFILE_
#include<iostream>
#include<iomanip>
#include<algorithm>
#include<math.h>
#include<memory.h>
#endif

using namespace std;

#ifndef _HUGEMODEL_
#define _HUGEMODEL_
#include"HugeModel.h"
#endif


// 巨大数的除法依赖于巨大数减法 到目前为止 减法完成的巨大数除法是最快的
// 如果后续发现 效率更高的方式解决巨大数除法问题 还会更改
HUGENUMBER* HUGENUMBER::div(HUGENUMBER val) {
	NUMBERMODEL valModel = {0};
	char temp_sign = {0};			//临时记录除法计算的商

	valModel = val.getNumberModel();
// 在进行除法之前 将被除数和除数的符号位都置位正号 方便加减法的进行 所以就需要上面的提前计算好结果的符号位
	temp_sign = this->numberModel.number_sign ^ valModel.number_sign;
	this->numberModel.number_sign = valModel.number_sign = POSTIVE_SIGN;	//统一将符号位改为正好 方便减法运算

	if(this->compareTo(val) == LESS_THAN) {			// 如果被除数小于除数 直接返回商为0 余数就是被除数
// 尽管除法的结果是0 但还是申请了1个长度的数组
		this->numberModel.number_count = 1;
		KwenDelete(this->numberModel.number_array);
		this->numberModel.number_array = NewInt(this->numberModel.number_count);

		return this;
	}
//调用迭代减法
	this->iterationSubtracting(val);
// 符号位的赋值
	this->numberModel.number_sign = temp_sign;

	return this;
}

void HUGENUMBER::iterationSubtracting(HUGENUMBER val) {
	HUGENUMBER *divResult = new HUGENUMBER();
	HUGENUMBER *biggerDivsorHug = new HUGENUMBER();
	HUGENUMBER *amplification;

// 这一部分是除法的核心代码 div = a / b 外层的作用在于当被除数小于除数的时候 整个除法也就结束了
// 所以这里的判断compareTo函数是小于等于的情况返回结果位true
	while(val.compareTo(*this) == true) {  //a/b 当a小于b的时候就结束  也就是this小于val

// 通过计算分析被除数和除数的得到一个放大倍数 用来放大除数
		amplification = this->getMinDivResult(val);

// 这里将倍数拷贝给temp 因为这个倍数在累加商的时候 会用到 需要提前保存 而且是深拷贝
		HUGENUMBER temp(*amplification);

// 放大除数 注意这里没有用val去调用乘法函数  一旦用val调用乘法函数 会改变val的值 影响下一次得到倍数
		biggerDivsorHug = amplification->mul(val);

// 内层的循环就是 不断的用被除数去减放大之后的除数 减的同时 商也增加
// 这个循环结束的条件同样是当被除数小于除数的时候  也就是*this < biggerDivsorHug
		while(biggerDivsorHug->compareTo(*this) == true) {
			divResult->add(temp);
			this->sub(*biggerDivsorHug);
		}

//这里的释放内存很容易被忽略掉 在上一次的c语言完成的时候 这里就被忘却了
		delete amplification;
	}
	delete biggerDivsorHug;

// this指向记录商的对象 这里divresult会自动调用析构函数 释放数组空间  那么this的指向 不就指向了空 这里不对
// 应该交换this和divResult所指向的结构体 让析构函数最后释放掉原来this的所指向那个数组空间
// 而this的数组已经指向了divResult的数组 所以析构函数释放之后不会影响this
 	NUMBERMODEL temp = divResult->getNumberModel();
	divResult->setNumberModel(this->numberModel);
	// 这里一定要用set方法设置交换其model切记 而不是这样：divResult.getNumberModel()= this->numberModel;
	this->numberModel = temp;
	delete divResult;

	eliminateZeros(this->numberModel);
}

// 确定每次放大除数可以最大放大多少倍数
// 通过计算得到除数乘的最大倍数 10的某次方
// 比如252 5162 5162 和 142
// 那么142可以放大的最大倍数就是1 0000 0000 放大至如下
// 252 5162 5162
// 142 0000 0000
// 再放大便会超过被除数  而这个1 0000 0000 如何确定的就是该函数的主要职责
HUGENUMBER* HUGENUMBER::getMinDivResult(HUGENUMBER val) {
	HUGENUMBER *amplification = new HUGENUMBER();  //倍数是一个10的某次方
	NUMBERMODEL temp_model = {0};
	int different_length; //是补几个0

// 首先得到两巨大数长度之差 以88行代码为例子 之差为8 那么就是要跟8个0 还需后面进一步判断
	different_length = getHugenumberLength(this->getNumberModel()) - getHugenumberLength(val.getNumberModel());

// 如果出现了除数的最高位一个数字大于被除数同样位置的数字 那么长度之差就需要减一 因为
// 252 5162 5162  和 526长度之差也为8 但是正确的放大倍数应该是1000 0000而不是1 0000 0000
	if(!compareHighestNum(this->numberModel, val.getNumberModel())) {
		different_length--;
	}

	// 下面都是设置这个倍数的过程
	temp_model.number_sign = POSTIVE_SIGN;
	temp_model.number_count = (different_length + 4) / 4;
	temp_model.number_array = new int[temp_model.number_count];
	memset(temp_model.number_array, 0, sizeof(int) * temp_model.number_count);

	for(int i = 0; i < temp_model.number_count - 1; i++) {
		temp_model.number_array[i] = 0;
	}

	const int formatZeros[] = {1,10,100,1000};
	temp_model.number_array[temp_model.number_count - 1] = formatZeros[different_length % 4];
	amplification->setNumberModel(temp_model);

	return amplification;
}

// 取余还是建立在除法之上的步骤 最后的剩下this结果就是余数结果 没什么好写注释的 可以看懂除法就可以看懂取余
HUGENUMBER* HUGENUMBER:: mod(HUGENUMBER val) {
	NUMBERMODEL valModel = {0};
	char temp_sign = {0};			//临时记录除法计算的商

	valModel = val.getNumberModel();
	temp_sign = this->numberModel.number_sign ^ valModel.number_sign;
	this->numberModel.number_sign = valModel.number_sign = POSTIVE_SIGN;	//统一将符号位改为正好 方便减法运算
	if(this->compareTo(val) == LESS_THAN) {																// 如果被除数小于除数 直接返回商为0 余数就是被除数
		this->numberModel.number_count = 1;
		KwenDelete(this->numberModel.number_array);
		this->numberModel.number_array = NewInt(this->numberModel.number_count);

		return this;
	}
	HUGENUMBER divResult;
	HUGENUMBER *biggerDivsorHug = new HUGENUMBER();
	HUGENUMBER *amplification;
	const int sleepTime = 0;

	while(val.compareTo(*this) == true) {  //a/b 当a小于b的时候就结束  也就是this小于val
		amplification = this->getMinDivResult(val);
		HUGENUMBER temp(*amplification);
		biggerDivsorHug = amplification->mul(val);
//如果这里 用指针去放大倍数amp会导致amp也变大 没法进行后续的商的累加 所以进行一次深拷贝 保留倍数temp
		while(biggerDivsorHug->compareTo(*this) == true) {
			// Sleep(sleepTime);
			divResult.add(temp);
			this->sub(*biggerDivsorHug);
		}
		delete amplification;
	}
	delete biggerDivsorHug;
	eliminateZeros(this->numberModel);

	return this;
}


// 求一个巨大数的幂次方 可以用快速幂算法 这里仅实现了幂指数为正的情况
// 对于幂指数为负的情况 需要开根号 还没有能力实现
HUGENUMBER* HUGENUMBER::hug_pow(int exponent) {
	if(exponent <= 0) {
		// 统一把指数为负数 0 的情况返回1
		this->numberModel.number_count = 1;
		KwenDelete(this->numberModel.number_array);
		this->numberModel.number_array = NewInt(this->numberModel.number_count);
		this->numberModel.number_array[0] = 1;

		return this;
	}
// 初始化一个底数是1
	HUGENUMBER *ans = new HUGENUMBER();
	ans->getNumberModel().number_array[0] = 1;

	while(exponent) {
		if(exponent & 1) {
			// 说明此时的幂指数是奇数
			ans->mul(*this);
		}
		this->mul(*this);
		exponent = exponent >> 1;
	}
// ans才是真正最后的幂次方计算结果
// 这里 交换完了之后 delete释放的是this原来指向的空间 已经改变了ans的指向了
// 所以没有影响到this
	NUMBERMODEL temp = ans->getNumberModel();
	// ans->getNumberModel() = this->numberModel;
	ans->setNumberModel(this->numberModel);
	this->numberModel = temp;

	delete ans;
	eliminateZeros(this->numberModel);

	return this;
}

// 这里还需要扩容 改变this数组的大小 和加法相同的去改变
HUGENUMBER* HUGENUMBER::mul(HUGENUMBER MulNum) {
	NUMBERMODEL mulNumModel = {0};
	int tempLength;
	int *temp_array;


	mulNumModel = MulNum.getNumberModel();
	tempLength = this->numberModel.number_count + mulNumModel.number_count + 1;
	temp_array = NewInt(tempLength);
	for(int i = 0; i < mulNumModel.number_count; i++) {
		int carry_data = 0;
		int result_index = i;
		for(int j = 0; j < this->numberModel.number_count; j++) {
			int eachResult = this->numberModel.number_array[j] * mulNumModel.number_array[i] + carry_data;
			carry_data = (eachResult + temp_array[result_index]) / CURRENT_SYSTEM;
			temp_array[result_index] = (temp_array[result_index] + eachResult) % CURRENT_SYSTEM;
			result_index++;
		}
		temp_array[result_index] = carry_data;
	}
	this->numberModel.number_count = tempLength;
	this->numberModel.number_sign ^= mulNumModel.number_sign;
	KwenDelete(this->numberModel.number_array);
	this->numberModel.number_array = temp_array;
	eliminateZeros(this->numberModel);

	return this;
}


//去掉巨大数中高位的无效0数字，0000 0000 0023 5985 = 0023 5985
//思路：先调用getHugenumberLength(HUGENUMBER myNumber)方法得到巨大数有效数字长度
//根据长度申请新空间，赋值，销毁旧巨大数，返回newHugenumber。

//这个函数方法还有问题 对于处理120+ -120 返回的结果巨大数长度 应该是0
void HUGENUMBER::eliminateZeros(NUMBERMODEL &numberModel) {
	int actual_length = 0;
	NUMBERMODEL newNumber = {0};

	actual_length = getHugenumberLength(numberModel);
	newNumber.number_sign = numberModel.number_sign;
	newNumber.number_count = (actual_length + 3) / 4;
	newNumber.number_array = NewInt(newNumber.number_count);
	for(int i = 0; i < newNumber.number_count; i++) {
		newNumber.number_array[i] = numberModel.number_array[i];
	}

	KwenDelete(numberModel.number_array);
	numberModel = newNumber;
}


// 从巨大数高位开始逐个判断数字大小，
//1 如果除数的第一个最高位数字大于被除数最高位第一个数字，直接返回FASLE
//2 如果除数的第一个最高位数字小于被除数最高位第一个数字，直接返回TRUE
//如果除数的第一个最高位数字等于被除数最高位第一个数字最高位数字继续重复1 2
bool HUGENUMBER::compareHighestNum(NUMBERMODEL dividendHug, NUMBERMODEL divisorHug) {
	int num1_length;
	int num2_length;
	int judgeTimes;

	num1_length = getHugenumberLength(dividendHug);
	num2_length = getHugenumberLength(divisorHug);
	judgeTimes = num1_length > num2_length ? num2_length : num1_length;

	while(judgeTimes > 0) {
		int dividend_num = getHighestNum(dividendHug.number_array, num1_length);
		int divisor_num = getHighestNum(divisorHug.number_array, num2_length);
		if(divisor_num > dividend_num) {
//如果除数最高位数字大于被除数最高位数字 直接返回FASLE
			return false;
		}else if(divisor_num < dividend_num) {
//除数的第一个最高位数字小于被除数最高位第一个数字，直接返回TRUE
			return true;
		}else {
//除数的第一个最高位数字等于被除数最高位第一个数字最高位数字继续重复上述判断
			num1_length--;
			num2_length--;
		}
		judgeTimes--;
	}

	return true;
}

//每次返回巨大数最高位的一个数字如21 1526 3545，第一次返回2第二次返回1
int HUGENUMBER::getHighestNum(int *array, int hugeNumber_length) {

	if(hugeNumber_length % 4 == 0) {
		return array[getCount(hugeNumber_length) - 1] / THOUSAND;
	}else {
		int Denom = (int)pow(10.0, (double)((hugeNumber_length % 4) - 1));

		return (array[getCount(hugeNumber_length) - 1] / Denom) % 10;
	}
}


// 这个计算的方式是a= a + b将计算的结果返回了a 那么就需要扩容a的数组长度
HUGENUMBER* HUGENUMBER::add(HUGENUMBER Addend) {
	NUMBERMODEL addend_model = {0};
	int i;
	int carry_data = 0;
	int *temp_array;
	int tempLength;

	addend_model = Addend.getNumberModel();
	tempLength = max(addend_model.number_count, this->numberModel.number_count) + 1;
	temp_array = NewInt(tempLength);

// 这一部分的完成主要是通过mec补码完成 无论是加法还是减法 都当做加法来计算
	for(i = 0; i < tempLength; i++) {
		int eachResult = getMecCode(indexOf(addend_model, i), addend_model.number_sign)
						+ getMecCode(indexOf(numberModel, i), numberModel.number_sign);
		temp_array[i] = (eachResult + carry_data) % CURRENT_SYSTEM;
		carry_data = (eachResult + carry_data) / CURRENT_SYSTEM;													//进位数字
	}
	// 计算结果的符号位 是最后一次的进位数据和两个加数进位异或操作
	this->numberModel.number_sign ^= carry_data ^ addend_model.number_sign;
	this->numberModel.number_count = tempLength;

	// 考虑连续进位的情况
	for(i = 0; i < tempLength; i++) {
		int value = temp_array[i] + carry_data;
		temp_array[i] = getMecCode(value % CURRENT_SYSTEM, numberModel.number_sign);
		carry_data = value / CURRENT_SYSTEM;
	}
	KwenDelete(this->numberModel.number_array);
	this->numberModel.number_array = temp_array;
	eliminateZeros(this->numberModel);

	return this;
}

// 理解mec补码的前提下 减法也是加法完成的 a-b = a+ (-b);
HUGENUMBER* HUGENUMBER::sub(HUGENUMBER Subtraction) {
	NUMBERMODEL subtractModel = {0};

	subtractModel = Subtraction.getNumberModel();
	subtractModel.number_sign = !subtractModel.number_sign;
	Subtraction.setNumberModel(subtractModel);

	return add(Subtraction);
}

void HUGENUMBER::show() {

	if(numberModel.number_count == 0) {
		cout << "0000" << endl;
		return;
	}
	const char* sign[] = {"[+]","[-]"};
	int i;
	cout << sign[numberModel.number_sign];

	for(i = this->numberModel.number_count - 1; i >= 0; i--) {
		cout << setw(4) << setfill('0') << numberModel.number_array[i] << " ";
	}

	cout << endl;
}

// 初始化字符串到数组
void HUGENUMBER::initHugeNumber(string num) {
	if(num.length() <= 0) {

		return;
	}
	const int formatZeros[] = {0,3,2,1};
	int actual_length  = 0;

	actual_length  = charIsNum(num[0]) ? num.length() : num.length() - 1;
	numberModel.number_count = (actual_length + 3) / 4;
	numberModel.number_sign = num[0] == '-' ? NEGATIVE_SIGN : POSTIVE_SIGN;
	numberModel.number_array = NewInt(numberModel.number_count);
	if(!charIsNum(num[0])) {
		// 移除字符串第一个符号位 只保留数字字符串纯数字的部分
		num.erase(num.begin(), num.begin() + 1);
	}
	num.insert(0,formatZeros[actual_length % 4], '0');  //填充0补齐 成长度为4的倍数

	actual_length = actual_length + formatZeros[actual_length % 4];
	for(int i = numberModel.number_count - 1; i >= 0; i--) {
		numberModel.number_array[i] = atoi(num.substr(actual_length - (i + 1) * 4, 4).c_str());
	}
}


HUGENUMBER::HUGENUMBER(char num[]) {
	initHugeNumber(num);
}

HUGENUMBER :: HUGENUMBER(string num) {
	initHugeNumber(num);
}

HUGENUMBER :: HUGENUMBER() {
	numberModel.number_array = NewInt(1);
	numberModel.number_count = 1;
	numberModel.number_sign = POSTIVE_SIGN;
}

bool HUGENUMBER::charIsNum(char ch) {
	return ch >= '0' && ch <= '9';
}

void HUGENUMBER::SetHugeNumber(string num) {
	initHugeNumber(num);
}


// 深拷贝方法
HUGENUMBER::HUGENUMBER(const HUGENUMBER &val) {
	numberModel.number_count = val.numberModel.number_count;
	numberModel.number_sign = val.numberModel.number_sign;
	numberModel.number_array = NewInt(numberModel.number_count);

	for(int i = 0; i < numberModel.number_count; i++) {
		numberModel.number_array[i] = val.numberModel.number_array[i];
	}
}
// 自动局部变量的构造函数是在程序的执行到达定义这个对象的位置时调用的，
  // 而对应的析构函数是在程序离开这个对象的作用域时调用的（即定义这个对象的代码完成了执行）。
  // 每次执行进入和离开自动对象的作用域时，
  // 都会调用它的构造函数和析构函数。如果程序调用了exit或abort函数而终止，则不会调用自动对象的析构函数。
HUGENUMBER :: ~HUGENUMBER() {
	// cout << "destor function :" << numberModel.number_array[0] << endl;
	KwenDelete(numberModel.number_array);
}

// 封装一个销毁释放内存函数 减少代码的重写
void HUGENUMBER::KwenDelete(int* &array) {
	delete[] array;
	array = NULL;
}

// 封装一个申请内存函数 附带对内存的初始化
int* HUGENUMBER::NewInt(int length) {
	int *array = NULL;

	array = new int[length];
	memset(array, 0, sizeof(int)*length);

	return array;
}

int HUGENUMBER::getMecCode(int value, bool number_sign) {

	return (number_sign == NEGATIVE_SIGN) ? (MAX_BIT - value) : value;;
}

int HUGENUMBER::indexOf(NUMBERMODEL myNumber, int current_Index) {

	return (current_Index >= myNumber.number_count) ? MIN_BIT : myNumber.number_array[current_Index];
}

void HUGENUMBER::setNumberModel(NUMBERMODEL model) {
	this->numberModel = model;
}

NUMBERMODEL HUGENUMBER::getNumberModel() {

	return numberModel;
}

int HUGENUMBER::getCount(int len) {
	return (len + 3) / 4;
}

// this 和val作比较 小于等于返回true 说明当前迭代的减法可以继续
// 也就是除数小于等于被除数
// 否则则返回false 等于
// 主要是由于除法使用减法迭代完成的  判断除数和被除数大小 传递进来之前会将除数和被除数都变为正的
// 判断两个数的大小问题 可以先从长度进行判断 这里指的是实际长度而不是数组长度
// 由于在除法的迭代相减的时候没有处理最高位无效数字0的问题 若用数组长度判断是不准确的
//  从而不用减法的判断
bool HUGENUMBER::compareTo(HUGENUMBER val) {
	int len1 = getHugenumberLength(this->numberModel);
	int len2 = getHugenumberLength(val.getNumberModel());

	if(len1 == len2) {
		int i;
		// 这个判断并不能得到结果 如果最高位相同的吗 需要进一步到更低的位比较
		for(i = this->getCount(len1) - 1; (i >= 0) && this->numberModel.number_array[i] == val.getNumberModel().number_array[i]; i--);
// 此时经过上一行的循环 比较当前数组两个下标处的大小即可
		bool result=this->numberModel.number_array[i] <= val.getNumberModel().number_array[i];

		return result;
	}

	return len1 <= len2;
}

//得到一个巨大数在十进制下的位数实际长度 不包含最高位为0的
//例如0000 02354 9563 4182实际长度为11
//为函数getAmplification(HUGENUMBER dividendHug, HUGENUMBER divisorHug)服务
//为函数eliminateZeros(HUGENUMBER *myNumber)服务
int HUGENUMBER::getHugenumberLength(NUMBERMODEL myNumber) {
	if(myNumber.number_count <= 1 && myNumber.number_array[0] == 0) {
		return 0;
	}
	const int array[] = {0, 10, 100, 1000, 10000};
	int i;
	int highestNum_length = 0;

	for(myNumber.number_count = myNumber.number_count - 1;
		myNumber.number_count >=0 && myNumber.number_array[myNumber.number_count] == 0;
		myNumber.number_count--);

	for(i = 0; i < 4; i++) {
		if(myNumber.number_array[myNumber.number_count] >= array[i] && myNumber.number_array[myNumber.number_count] < array[i+1]) {
			highestNum_length = i+1;			//记录巨大数最高位数字的位数
		}
	}

	int result = highestNum_length + (myNumber.number_count <= 0 ? 0 : myNumber.number_count) * 4;

	return result;
}
