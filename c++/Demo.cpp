#include<iostream>

using namespace std;

#include"./include/HugeModel.h"

int main() {
	string num1 = "+526325416";
	string num2 = "+33245";


	HUGENUMBER *hugeNum1 = new HUGENUMBER(num1);   //这个里面所有的成员变量都是在堆上面申请的 卧槽
	HUGENUMBER *hugenum2 = new HUGENUMBER(num2);

cout << "init infor:" << endl;
	hugeNum1->show();
	hugenum2->show();
	cout << endl;

	// hugeNum1->sub(*hugeNum2);
	// hugeNum1->add(*hugeNum2);
	// hugeNum1->mul(*hugenum2);
	hugeNum1->div(*hugenum2);
	// hugeNum1->mod(*hugenum2);
	// hugeNum1->hug_pow(3);

cout << "after:" << endl;
	hugeNum1->show();
	hugenum2->show();

	delete hugeNum1;
	delete hugenum2;

	return 0;
}
