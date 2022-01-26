//======================C++_Primer书店程序======================
//为什么要学习书店程序？
//书店程序是C++_Primer经典用例，使用c++11语言，面向对象的思想解决实际问题
//该程序涵盖了c++语言的众多必备知识点，是对理解和掌握c++的最佳实践之一
//该程序包含的c++知识点(可能有未总结到的)：
//一.基础知识
//	引用
//	指针
//	顶层const，底层const
//	函数，实参和形参，实参的初始化
//	初始化，赋值
//	直接初始化,拷贝初始化
//二.类
//	构造函数,初始值列表，内类初始值，构造函数执行流程, 成员初始化顺序
//	默认构造函数，合成的默认构造函数,default
//	拷贝构造函数，拷贝赋值运算符,合成的版本
//	构造函数的explicit显示调用，隐式转换
//	成员函数，inline内联函数，隐式this指针，const限定符
//	运算符重载,成员函数，非成员函数
//	重载的调用运算符(),函数对象
//	名字查找与类的作用于page 254
//	类的访问修饰符
//
//三.模板
//	函数模板
//	类模板
//	模板的实例化
//	特例化
//四.友元
//	友元，类，模板在应用中的关系
//五.IO
//	标准输入，标准输出，标准错误
//	cin,cout,cerr
//六.命名空间
//	using声明，using指示
//	命名空间中的名字查找
//	名字冲突，命名空间污染
//七.头文件/源文件
//	声明和定义
//	什么时候定义在头文件，什么时候定义在源文件
//	模板特例化的影响
//八.异常处理
//	throw和catch,throw异常对象和catch异常声明与静态编译类型的关系
//	异常查找和匹配流程
//	异常处理流程
//	被析构的代码
//	异常处理后代码恢复执行点


//书店程序简介：
//一个用来记录书的销售记录的应用程序
//Book_sale类是销售记录的抽象数据类型,它包含以下关键成员
//bookNo:书本编号，代表一本书籍
//units_sold:代表bookNo对应的这本书的销售数量
//revenue:代表这本书的销售总额
//注意每个Book_sale对象记录的是bookNo对应的这一本书的销售记录
//一个书店会产生很多这样的记录，比如每天的记录，每个月的记录，每年的记录
//因此每个具有相同bookNo的Book_sale对象可以相加以合并这些相同书籍的销售记录

#ifndef BOOK_SALE_H
#define BOOK_SALE_H

#include <string>
#include <iostream>

//前置声明，hash友元声明需要 page 628 589
//!!!实际测试中发现前置声明导致编译错误，去掉则没问题？？？
//template<class T> class std::hash;
class Book_sale {
	//我们将会特例化hash<Book_sale>模板，为无序容器提供对Book_sale类的支持
	//将hash设置为友元，hash模板将使用Book_sale私有数据
	//本代码友元声明对应关系：只有用Book_sale实例化的hash才是Book_sale的友元
	//这样关系的友元声明需要前置声明 page 589
	//如果形如这样的：template<typename T> friend class std::hash;
	//代表所有hash模板实例都是Book_sale的友元，则无需前置声明
	//非模板类的友元声明也无需前置声明，如friend class Myclass;
	friend class std::hash<Book_sale>;
	
	//重载运算符设置为友元,非成员函数
	//为了和标准库iostream兼容输入输出运算符函数必须是非成员函数
	//如果是成员函数，左侧运算对象绑定到第一个参数作为this指针
	//于是输出变成book << cout,而我们要的效果是cout << book，这显然不对
	//注意输入的内容是：书本编号，销售数量，销售单价
	friend std::ostream &operator<<(std::ostream &, const Book_sale &);
	friend std::istream &operator>>(std::istream &, Book_sale &);
	//相等运算符函数应该是非成员函数
	friend bool operator==(const Book_sale &, const Book_sale &);
	
	//重载运算符的普通函数版本设置为友元,非成员函数
	friend std::ostream &print(std::ostream &, const Book_sale &);
	friend std::istream &read(std::istream &, Book_sale &);

public:
	//构造函数
	//使用合成的默认构造函数，之所以需要这样声明是因为我们定义了其他构造函数，
	//默认构造函数会被隐藏,而我们又需要默认构造函数
	//合成的默认构造函数首先使用类内初始值初始化成员，如果没有提供类内初始值则执行默认初始化
	//默认初始化：
	//对于内置类型，定义在函数体外的变量被初始化为0，定义在函数体内的不被初始化，不被初始化的内置类型的值是未定义的
	//对于类类型，每个类自己决定对象的初始化
	
	//注意Book_sale有合成的拷贝构造函数和拷贝赋值运算符
	Book_sale() = default;

	//:和{}之间的部分是构造函数的初始值列表，用于初始化成员
	//注意初始化和赋值是完全不一样的概念
	//初始化是定义变量并赋予初值，赋值是将一个值赋予一个已经初始化过的变量
	//注意构造函数先初始化成员，然后再执行函数体,这意味着只有初始值列表和类内初始值的方式才可以初始化成员
	//注意成员的初始化顺序和类中定义的顺序一致，和初始值列表中的顺序无关
	//函数体内执行的形如this->bookNo = s这种形式都属于赋值，
	//这样做是不对的，一方面是赋值通常需要拷贝一次，并且const的变量无法重新赋值
	//下面的构造函数使用初始值列表初始化了bookNo成员，
	//units_sold和revenue被忽略了，将按照合成默认构造函数相同的方式初始化，使用类内初始值初始化
	//假如bookNo不在初始值列表中，也没有类内初始值，它将被默认初始化，string类控制初始化为空字符串
	//假如我们还定义了一个没有类内初始值的int value,则执行默认初始化，将初始化为0
	Book_sale(const std::string &s) : bookNo(s) {}
	Book_sale(const std::string &s, unsigned n, double p) :
		bookNo(s), units_sold(n), revenue(p*n) {} 
	Book_sale(std::istream &);
	//获取书本编号,成员函数
	std::string isbn() const { return bookNo; }
	//重载+=运算符，两个Book_sale对象相加,成员函数
	//赋值运算符=必须是成员函数，复合赋值运算符+=通常也是成员函数
	//为了和内置类型的含义保持一致，=和+=都应该返回左侧运算对象的引用
	Book_sale &operator+=(const Book_sale &);

private:
	//const是一个地层const,指定隐式this指针所指的对象是const的
	//而this指针本身默认是const的，简单来说就是：
	//没有const修饰 Book_sale *const this
	//有constx修饰  const Book_sale *const this
	//因此avg_price函数不能改变调用对象的值，
	//但是如果没有const修饰，const对象将无法调用avg_price函数
	double avg_price() const; //计算平均价格
	std::string bookNo;       //书的编号,没有类内初始值
	unsigned units_sold = 0;  //销售量,类内初始值为0
	double revenue = 0.0;     //销售总额,类内初始值为0.0
};

//定义hash模板的特例化版本
//当不能或不希望使用模板版本时，定义特例化版本
//模板特例化必须定义在原模板的命名空间中
//为了特例化一个模板，源模板的声明必须在作用域中，好像没有体现?命名空间std也是一个作用域的缘故?
//特例化hash规定：
//1.一个重载的调用运算符(),它接受容器关键字类型的对象，返回一个size_t
//2.两个类型成员，result_type和argument_type,分别为调用运算符的返回类型和参数类型
//3.默认构造函数和拷贝赋值运算符（可以是隐式的)
//为了让Book_sale的用户使用hash<Book_sale>特例化版本，所以我们应该在Book_sale的头文件中定义特例化版本，如下所示
//特别注意：
//1.函数模板在使用时才被实例化
//2.类模板在定义时被实例化，如vector<string> books;就实例化了一个vector<string>的类
//3.默认情况下，对于一个实例化的类模板，其成员只有在使用时才被实例化
//4.特例化的本质是实例化一个模板
//5.page 582:为了生成一个实例化版本，编译器需要掌握函数模板或类模板成员函数定义,因此通常函数模板和类模板成员函数都定义在头文件，然而并非必须这样：
//当我们在头文件特例化hash<Book_sale>时就是实例化了hash<Book_sale>,但是并没有实例化hash<Book_sale>::()成员函数，因此头文件中没有hash<Book_sale>::()的定义编译也不会出错
namespace std {

template<>
struct hash<Book_sale>
{
	typedef size_t result_type;
	typedef Book_sale argument_type;

	size_t operator()(const Book_sale &s) const;
};

}	

//inline: 内联函数，在调用点展开，减少函数调用开销
//通常在函数逻辑简单、行数少且调用频繁时使用
//内联函数必须在定义处指定inline关键字，函数声明中也可以同时指定inline，但是为了可阅读性通常声明中不指定
//内联函数和普通函数不一样，内联函数可以多次定义，但是定义必须完全一样，通常内联函数直接定义在头文件中

//普通比较函数，非成员函数
//Book_sale类不存在一种逻辑可靠的<定义，因为从用户角度来说，两个对象的大小比较比较的是什么是不确定的，
//是不唯一的，所以我们不为其定义重载的<关系运算符，而是定义一个通过书本编号进行比较的普通函数
inline
bool compareIsbn(const Book_sale &lhs, const Book_sale &rhs) {
	return lhs.isbn() < rhs.isbn();
}

//运算符重载的比较函数，非成员函数
//相等运算符通常定义为非成员函数
inline
bool operator==(const Book_sale &lhs, const Book_sale &rhs) {
	return lhs.isbn() == rhs.isbn() &&
	       lhs.units_sold == rhs.units_sold &&
	       lhs.revenue == rhs.revenue;
}

//运算符重载的比较函数，非成员函数
//定义了==，通常也需要定义!=，而!=依托于==
inline 
bool operator!=(const Book_sale &lhs, const Book_sale &rhs) {
	return !(lhs == rhs);
}

//注意命名空间、友元声明和实参相关查找有这样的规则:
//1.当我们给函数传递类类型的对象时，除了在常规作用域查找外还会查找实参类所属的命名空间
//2.未声明的类或函数如果第一次出现在友元声明中，我们认为它是最近的外层命名空间的成员
//因此理论上来说，下面在Book_sale中声明为友元的非成员函数，在这里不声明在调用时也可以查找到？todo有待验证

//普通函数版本接口声明,非成员函数
Book_sale add(const Book_sale &, const Book_sale &);
std::ostream &print(std::ostream &, const Book_sale &);
std::istream &read(std::istream &, Book_sale &);
//运算符重载接口声明,非成员函数
//算术运算符+具有对称性,通常定义为非成员函数
Book_sale operator+(const Book_sale &, const Book_sale &);
std::ostream &operator<<(std::ostream &, const Book_sale &);
std::istream &operator>>(std::istream &, Book_sale &);

#endif
