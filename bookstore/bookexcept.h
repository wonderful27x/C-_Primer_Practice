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
	//例子:
	//A: out_of_stock e(string("error")); out_of_stock ee = e; 将调用拷贝构造函数
	//B: out_of_stock e = string("error"); 不会调用拷贝构造函数???，而是隐式调用普通构造函数
	//C：out_of_stock e = out_of_stock(string("error")); 也不会调用拷贝构造函数???，只有A才会调用拷贝构造函数
	//注意: 
	//	上面的例子是通过实际测试得出的结论，在拷贝构造函数体中打印信息，只有A才会打印，B和C都不会打印，
	//	然而当把拷贝构造函数定义为删除的函数，ABC都将编译失败，这似乎又不能说明BC没有调用拷贝构造函数,将拷贝构造函数定为explicit AC编译失败
	//	拷贝初始化依靠拷贝构造函数(或移动构造函数),书本page 441原话，好好体会
	//	另外将拷贝构造函数和拷贝赋值运算符定义为删除的函数将阻止拷贝
	
	//设: void showError(const out_of_stock e)
	//没有explicit修饰：
	//隐式的类类型转换：能通过一个参数调用的构造函数定义了一条从构造函数的参数类型向类类型隐式转换的规则
	//在需要此类型对象的时候可以用这个构造函数的参数类替代,编译器会使用这个参数自动构建临时对象
	//out_of_stock error_sale(string("error"));  正确，直接初始化
	//out_of_stock error_sale = string("error"); 正确，隐式调用
	//showError(string("error"));                正确，隐式调用
	
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
