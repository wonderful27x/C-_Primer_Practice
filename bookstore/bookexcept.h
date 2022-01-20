#ifndef BOOKEXCEPT_H
#define BOOKEXCEPT_H

#include <stdexcept>
#include <string>

//书店异常类
//运行时异常
class out_of_stock : public std::runtime_error {
public:
	//使用=号初始化的是拷贝初始化，否则是直接初始化，注意是初始化不是赋值
	//拷贝初始化依靠拷贝构造函数(或移动构造函数)完成
	//拷贝初始化在以下情况发生：
	//1.使用=定义变量(初始化)
	//2.将一个对象作为实参传递给一个非引用类型的形参
	//3.从一个返回类型为非引用类型的函数返回一个对象
	//4.用花括号列表初始化一个数组中的元素或一个聚合类中的成员
	//如out_of_stock e(string("error")); out_of_stock ee = e;
	
	//没有explicit修饰：
	//隐式的类类型转换：能通过一个参数调用的构造函数定义了一条从构造函数的参数类型向类类型隐式转换的规则
	//在需要此类型对象的时候可以用这个构造函数的参数类替代,编译器会使用这个参数自动构建临时对象
	//情况一：
	//void showError(const out_of_stock &e)
	//showError(string("error"));
	//实参初始化相当于：const out_of_stock &e = string("error");
	//这个时候实际就会隐式调用构造函数构建一个临时对象out_of_stock(string("error")); 
	//然而值的注意的是这种情况并不属于上述的拷贝初始化,因为参数是引用
	//情况二：
	//out_of_stock error_sale = string("error");
	//这种情况就属于拷贝初始化,将右侧运算对象拷贝到正在创建的对象中，
	//然而右测不是一个out_of_stock对象，所以会隐式调用out_of_stock(string("error")); 
	//最终大概会是这样out_of_stock error_sale(out_of_stock(string("error"));
	
	//有explicit修饰
	//explicit只对一个参数的构造函数有效，类内声明构造函数使用，类外定义时不能重复使用
	//explicit修饰指定显示调用构造函数，只能直接初始化，不能隐式转换
	//out_of_stock error_sale(string("error"));  正确，直接初始化
	//out_of_stock error_sale = string("error"); 错误，explicit修饰的含有一个string参数的构造函数不能隐式调用
	//showError(string("error"));                错误，explicit修饰的含有一个string参数的构造函数不能隐式调用
	explicit out_of_stock(const std::string &s) : 
		std::runtime_error(s) {}
};

//逻辑异常
class isbn_mismatch : public std::logic_error {
public:
	explicit isbn_mismatch(const std::string &s) : 
		std::logic_error(s) {}
	isbn_mismatch(const std::string &s, const std::string &lhs, const std::string &rhs) : 
		std::logic_error(s), left(lhs), right(rhs) {}
	const std::string left,right;
};

#endif
