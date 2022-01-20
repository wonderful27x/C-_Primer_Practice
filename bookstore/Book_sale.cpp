//书店程序实现部分
#include <string>
#include <iostream>

#include "Book_sale.h"

//using声明，一次引入命名空间中一个成员
//作用范围：从声明开始到声明所在作用域结束,
//因此头文件通常不使用using而直接使用命名空间，否则include时就容易造成命名冲突
using std::istream; 
using std::ostream;


//重载的运算符实际上就是一个名为operatorxxx的函数,它也有自己的参数和返回值
//一元重载运算符函数有一个参数
//二元重载运算符函数有两个参数，左侧运算对象是第一个参数，右侧运算对象为第二个
//一个二元重载运算符函数如果是成员函数，那么以左侧运算对象的第一个参数是隐式的
//这是因为c++成员函数第一个参数是一个隐式的this指针，成员的使用实际是隐式的使用this指向的成员
//如units_sold实际等价于this->units_sold
//想象一下：
//Book_sale a,b;
//a += b; 等价于 a.operator+=(b);
//相当于调用a对象的operator+=这个函数，a作为this指针隐式传递给第一个参数
//注意这个函数会抛出异常，调用这个函数的函数应该注意
Book_sale &Book_sale::operator+=(const Book_sale &rhs) {
	if(isbn() != rhs.isbn())
		throw isbn_mismatch("wrong isbns", isbn(), rhs.isbn());
	units_sold += rhs.units_sold;
	revenue += rhs.revenue;
	return *this;
}

//非成员函数，所以不需要作用域Book_sale::
//因为是创建了一个局部变量Book_sale,返回局部变量的引用或指针是严重的错误(离开作用域的变量将被销毁)，
//所以返回值是Book_sale,相当于返回局部变量的一个副本
Book_sale operator+(const Book_sale &lhs, const Book_sale &rhs) {
	//注意sum是局部变量，非引用或指针
	//拷贝lhs数据到sum中,显然这里使用了合成的拷贝构造函数
	//注意这句代码是初始化而不是赋值，所以是调用拷贝构造函数而不是拷贝赋值运算符
	Book_sale sum = lhs;
	//调用重载复合赋值运算符
	//也可以通过函数名调用：
	//sum.operator+=(rhl);
	sum += rhs;
	return sum;
}	

//hash特例化成员函数的实现
namespace std {

//hash对于string和内置类型有可以直接使用的版本，自定义的类型则没有
//所以我们需要特例化hash<Book_sale>,而这里我们直接利用内置版本，
//实际就是直接调用hash<T>的对象，拥有重载运调用运算符()的对象就是一个函数对象(可调用对象)
size_t hash<Book_sale>::operator()(const Book_sale &s) const {
	return hash<string>()(s.bookNo) ^
	       hash<unsigned>()(s.units_sold) ^
	       hash<double>()(s.revenue);
}

}

//从输入流中构建一个Book_sale对象
Book_sale::Book_sale(istream &is) {
	is >> *this;
}

//计算平均价格
double Book_sale::avg_price() const {
	if(units_sold)
		return revenue/units_sold;
	else
		return 0;
}

//重载输入运算符，从输入流构建一个Book_sale对象
//返回istream,使得输入可以像istream >> sale1 >> sale2这样使用
//注意输入的内容是：书本编号，销售数量，销售单价
istream &operator>>(istream &is, Book_sale &item) {
	double price;
	is >> item.bookNo >> item.units_sold >> price;
	if(is)
		item.revenue = item.units_sold * price;
	else
		//如果输入失败，赋予一个default的Book_sale对象
		//注意这里并不是要将引用item绑定到另一个对象(这是不允许的),这里就相当于通过引用改变对象的值
		//只不过这里实际是在调用合成的拷贝赋值运算符
		item = Book_sale();
	return is;
}

//重载输出运算符
ostream &operator<<(ostream &os, const Book_sale &item) {
	os << item.isbn() << " " << item.units_sold << " "
	   << item.revenue << " " << item.avg_price();
	return os;
}	

//普通函数版本输入
istream &read(istream &is, Book_sale &item) {
	double price = 0;
	is >> item.bookNo >> item.units_sold >> price;
	item.revenue = price * item.units_sold;
	return is;
}

//普通函数版本的输出
ostream &print(ostream &os, const Book_sale) {
	os << item.isbn() << " " << item.units_sold << " "
	   << item.revenue << " " << item.avg_price();
	return os;
}

//普通函数版本的+
Book_sale add(const Book_sale &lhs, const Book_sale &rhs) {
	Book_sale sum = lhs;
	sum += rhs;
	return sum;
}
