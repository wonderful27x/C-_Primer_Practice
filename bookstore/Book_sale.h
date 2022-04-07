//======================C++_Primer书店程序======================
//为什么要学习书店程序？
//书店程序是C++_Primer经典用例，使用c++11语言，面向对象的思想解决实际问题
//该程序涵盖了c++语言的众多必备知识点，是对理解和掌握c++的最佳实践之一
//该程序包含的c++知识点(可能有未总结到的)：
//一.基础知识
//	c++基础数据类型
//	类型转换
//	引用
//	指针
//	顶层const，底层const
//	函数，实参和形参，实参的初始化
//	初始化，赋值
//	直接初始化,拷贝初始化
//二.类
//	构造函数,初始值列表，内类初始值，构造函数执行流程, 成员初始化顺序
//	默认构造函数，合成的默认构造函数,默认构造函数调用时机,default
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
	//	* 重载的运算符时具有特殊名字的函数: return_type operator_xxx(type a, type b)
	//      * 对于二元的成员运算符，第一个参数是调用对象的隐式this指针，因此显式参数只有一个
	//      * 定义为成员还是非成员的一般规则
	//      	* 赋值=，下标[],调用()和成员访问->,必须是成员函数
	//      	* 复合赋值运算符一般是成员
	//      	* 该变对象状态或与给定类型密切相关的，如递增、递减、解引用应该是成员
	//      	* 具有对称性的运算符可能转换任意一端的运算对象，如算术、相等性、关系和位运算，通常是非成员函数 
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
	//默认构造函数调用时机: page 262
	
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


//=================================================附录===========================================
//函数
//	* 实参是形参的初始值
//	* 自动对象: 
//		* 函数经过变量定义语句创建对象，函数结束销毁对象，形参是自动对象
//		* 因此不要返回局部对象的引用或指针
//	* 局部静态对象static: 函数第一次经过变量定义语句时初始化，直到整个程序结束才销毁
//	* 函数匹配三部曲
//		* 确定重载函数集: 函数同名，声明在调用点可见
//		* 确定可行函数: 形参与实参数量相等，实参与形参类型对应或可以转换成形参类型
//		* 最佳匹配: 实参与形参类型越接近匹配得越好
//
//基础数据类型
//	类型		含义			最小尺寸
//	bool		布尔类型		未定义
//	char		字符			8
//	wchar_t 	宽字符			16
//	char16_t	Unicode字符		16
//	char32_t	Unicode字符		32
//	short		短整形			16
//	int 		整形			16
//	long 		长整形			32
//	long long 	长整形			64
//	float		单精度浮点数		6位有效数字
//	double		双精度浮点数		10位有效数字
//	long double	扩展精度浮点数		10有效数字
//
//	* 字节: 可寻址的最小内存块,C++要求一个字节至少能容纳机器基本字符集中的字符，大多数为8位
//	* 字  : 存储的基本单元，由多个字节组成，大多数为32或64位
//	* c++要求一个int至少和一个short一样大，一个long至少和一个int一样大，一个long long至少和一个long一样大
//	* char的特殊性: char 表示无符号还是有符号由编译器决定
//
//类型转换
//	* 非bool算术值赋值给bool，0为false，否则为true
//	* bool赋值给非bool，false为0,true为1
//	* 浮点赋值给整数，保留小数点之前部分
//	* 整数赋值给浮点数，小数部分记作0,整数所占空间超过浮点数容量精度可能丢失
//	* 给无符号类型一个超过其表示范围的值，结果是初始值对无符号类型表示值总数取模后的余数
//		unsigned char c = -1; //c为255
//	* 给带符号类型一个超过其表示范围的值，结果是未定义的
//	* 含有无符号类型的表达式: 算术运算既有无符号，又有有符号的int，int会首先转换为无符号类型
//	* 10进制字面值是带符号的，8和16进制可能带符号也可能不带符号
//
//	* 算术类型转换
//		* 表达式既有浮点类型也有整数类型，整数转成相应的浮点类型
//		* 整型提升
//			* 小整型: bool、char、signed char、unsigned char、short、unsigned short如果能存下转成int，否则转成unsigned int
//			* 较大整型: wchar、char16_t、char32_t，提升成int、unsigned int、long、unsigned long、long long、unsigned long long中最小的
//		* 无符号运算对象
//			* 先执行整数提升
//			* 提升后两个运算对象同符号，小的将转换成大的
//			* 提升后异号，无符号类型不小与带符号，带符号的转换无符号的
//			* 提升后异号，带符号大与无符号，如果无符号类型所有值都能存在有符号类型中，则无符号转成带符号，否则带符号转成无符号
//		* 例子: 3.14L + 'a', 'a'先提升为int，int再转成long double
//	* 强制类型转换(page 145 730)
//		* static_cast: 任何有明确定义的类型转换，只要不包含底层cosnt都可以，将较大算术类型赋值给较小类型时使用，意味着告诉编译器和读者我承担精度损失风险
//		* cosnt_cast: 只能改变运算对象的底层cosnt，即去cosnt
//		* reinterpret_cast: 为运算对象的位模式提供较低层次上的重新解释
//		* dynamic_cast: 将基类指针或引用安全转换成派生类的指针或引用
//
//名字查找(page 254)
//	* 类的定义分两步:
//		* 首先编译成员的声明
//		* 直到类全部可见后才编译函数体
//	* 用于类成员声明中的名字查找
//		* 声明中使用的名字，包括返回类型或参数类型使用的名字都必须在使用前可见，如果成员声明使用了类中尚未出现的名字，则在定义类的作用域中继续查找
//	* 成员定义中的普通块作用域的名字查找
//		* 在函数体内使用名字之前的域查找
//		* 如果没有找到，在类内找，类的全部成员都被考虑
//		* 类内也没有找到，在函数定义之前的作用域找
//
//异常处理
//	* exception
//		* bad_cast
//		* bad_alloc
//		* runtime_error
//		* logic_error
//	* throw 表达式引发一个异常，throw后面的语句不在被执行
//	* 异常抛出与匹配查找流程
//		throw在try语句块中，当调用throw抛出一个异常后：
//		查找与try关联的catch是否有匹配的异常处理
//		如果没有找到,查找嵌套的外层try catch
//		如果还没找到，退出当前函数，对抛出异常的函数的调用语句所在的try catch查找
//		如果还没有找到，查找嵌套的外层try catch,
//		如果还没找到，退出当前主调函数，对调用主调函数的其他函数查找
//		...
//		如果最终都没找到，调用terminate函数终止程序
//		注意当没有查找到退出某个块时，块中创建的对象被销毁，析沟函数被调用
//	* 异常处理流程
//		查找到匹配的catch子句，使用throw抛出的对象初始化catch声明的异常对象，初始化过程和函数型参初始化几乎一样
//		执行其中的代码,处理当前情况，如果无法完全解决，修改异常对象(如果是引用类型才有意义),throw继续抛出
//		如果没有再次抛出，执行完后找到与try块关联的最后一个catch字句以后的点开始继续执行
//	* throw 一个表达式，表达式的静态编译类型决定异常对象的类型
//		例如表达式解引用一个基类指针，而指针指向派生对象，则抛出的对象被裁掉一部分只剩基类部分
//	* catch 异常声明的静态编译类型决定它能执行的操作
//		例如catch是基类引用类型，throw的是派生类，执行动态绑定，但是catch无法使用派生类的任何成员(强转不形吗？)
//		注意这时如果通过基类引用调用虚函数正常的调用到派生类的函数
//		如果catch声明的是非引用类型，相当于值传递，对象被切掉一部分，只剩基类部分
//	* noexcept: 承诺不会抛出异常，但如果运行时抛出了异常那是开发者的失误
//
//内存对齐
//	* 结构体中的内存对齐，很重要，结构体的大小由结构体的元素和排列顺序有关，因此在定义结构体是注意这些细节可以节约内存开销
//	* 结构体元素按定义顺序一一放到内存中，但排列并不是紧密排列的；
//	1. 元素为基本数据类型: 从结构体的首地址开始，每个元素都会认为内存是以自己的大小来划分的，因此元素放置的位置一定是自己宽度的整数倍。
//	2. 包涵指针: 将指针看成4个字节即可，无需关心指向的数据类型
//	3. 包含其他结构体: 结构体放置的初始位置是结构体中最宽元素（基本数据类型）长度的整数倍
//	4. 内存大小需是所有元素中最宽元素（基本数据类型）长度的整数倍，不够则补齐。
