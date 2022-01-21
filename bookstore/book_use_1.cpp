#include <iostream>

#include "Book_sale.h"
#include "bookexcept.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char **argv) {
	Book_sale item1,item2,sum;
	//从输入构造两个Book_sale
	cout << "请输入销售记录，格式： 书本编号(string) 销售数量(unsigned) 单价(double), ctrl+d 退出" << endl;
	while(cin >> item1 >> item2) {
		cout << "input 2 records: " << item1 << ", " << item2 << endl; 
		//+调用+=，+=有异常抛出语句
		//throw isbn_mismatch("wrong isbns", isbn(), rhs.isbn());
		//throw 表达式引发一个异常，throw后面的语句不在被执行

		//异常抛出与匹配查找流程
		//throw在try语句块中，当调用throw抛出一个异常后：
		//查找与try关联的catch是否有匹配的异常处理
		//如果没有找到,查找嵌套的外层try catch
		//如果还没找到，退出当前函数，对抛出异常的函数的调用语句所在的try catch查找
		//如果还没有找到，查找嵌套的外层try catch,
		//如果还没找到，退出当前主调函数，对调用主调函数的其他函数查找
		//...
		//如果最终都没找到，调用terminate函数终止程序
		//注意当没有查找到退出某个块时，块中创建的对象被销毁，析沟函数被调用

		//异常处理流程
		//查找到匹配的catch子句，使用throw抛出的对象初始化catch声明的异常对象，初始化过程和函数型参初始化几乎一样
		//执行其中的代码,处理当前情况，如果无法完全解决，修改异常对象(如果是引用类型才有意义),throw继续抛出
		//如果没有再次抛出，执行完后找到与try块关联的最后一个catch字句以后的点开始继续执行

		//throw 一个表达式，表达式的静态编译类型决定异常对象的类型
		//例如表达式解引用一个基类指针，而指针指向派生对象，则抛出的对象被裁掉一部分只剩基类部分
		//catch 异常声明的静态编译类型决定它能执行的操作
		//例如catch是基类引用类型，throw的是派生类，执行动态绑定，但是catch无法使用派生类的任何成员
		//注意这时如果通过基类引用调用虚函数正常的调用到派生类的函数
		//如果catch声明的是非引用类型，相当于值传递，对象被切掉一部分，只剩基类部分
		try {
			//将两个销售记录相加
			sum = item1 + item2;
			cout << "total book sales: " << sum << endl;
		}catch(const isbn_mismatch &e) {
			cerr << e.what() << ": left isbn(" << e.left
			     << ") right isbn(" << e.right << ")" << endl;
		}
	}
	cout << "total book sales: " << sum << endl;

	return 0;
}
