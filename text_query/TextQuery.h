//====================文本查询程序-查询部分=================
//为什么要学习文本查询程序？
//文本查询程序是C++_Primer经典用例，它的核心是教会我们如何使用标准库以及如何利用c++的继承体系,
//以面向对象的程序设计思想来解决复杂问>题，这也正是c++的精髓所在。

//文本查询程序简介：
//文本查询程序的目标是在给定的一篇文章中查询一个单词，并输出其在文章中出现的行号，并且程序支持~、&、|这样的复杂的复合查询，
//如~可以输出单词未出现的行号，&和|可以对两个对象的查询结果取交集和并集
//文本查询程序主要分为两大部分：
//一.查询部分，该部分是文本查询的主要实现，学习重点是标准库容器
//二.接口设计部分，该部分主要是设计并实现对一个或多个关键字的~、&、|等不同查询操作，学习重点是类的继承和程序设计模式

#ifndef TEXTQUERY_H
#define TEXTQUERY_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "QueryResult.h"

//声明result类，query的返回类型需要,todo 但是前面include中引入了还需要吗？
//名字查找page 254
//类的定义分两步：1.首先编译成员的声明 2.直到类全部可见才编译函数体
//类成员声明的名字查找
//	类的普通成员类型、函数的返回类和参数类型使用的名字必须使用前可见
//	如当编译器遇到query函数声明中的QueryResult时，它首先在TextQuery类内部使用QueryResult之前(即query函数声明之前),
//	查找QueryResult的声明，如果没有找到(TextQuery内部没有定义这样的类型)，
//	编译器到外层作用域(即TextQuery类外面)TextQuery类定义之前查找。
//成员函数中的名字查找
//	首先在成员函数内部查找(使用之前)
//	如果没有找到，则在类内部找，注意前面类的定义的2.直到类全部可见才编译函数体，所以类的所有成员都被考虑，包括成员函数
//	如果没有找到则在成员函数定义之前的作用域查找,注意是定义之前
class QueryResult;
class TextQuery {
public:
	//定义我们自己的类型，用于表示行号
	//using和typedef作用类似
	using line_no = std::vector<std::string>::size_type;
	TextQuery(std::ifstream &);
	QueryResult query(const std::string &) const;
	void display_map();

private:
	//注意头文件中我们通常都使用命名空间::域运算符而不使用using,
	//这时为了避免命名空间的污染，但是书写和阅读会比较麻烦
	
	//shared_ptr,智能指针，其他地方如QueryResult会用到，为了性能考虑，使用智能指针，而不是每次拷贝一个副本
	//为什么要使用智能指针而不是普通指针？
	//1.多次delete同一块内存引发错误(注意delete nullptr的指针不会出错，delete内存后将指针置空可以避免多次delete)
	//2.使用被delete的内存将发生错误
	//假设QueryResult和TextQuery的一个普通指针指向同一块内存，在没有特殊处理的情况下一定会出现1的情况，
	//还有可能出现2的情况,所以我们引入智能指针
	//智能指针是模板，实例化是类类型，它采用引用计数的方式帮我们管理指针
	//1.拷贝一个shared_ptr引用计数+1
	//2.用一个shared_ptr初始化另一个shared_ptr引用计数+1
	//3.作为参数传递给函数或作为函数返回值引用计数+1
	//4.赋值
	//	auto p = make_shared<int>(40)
	//	auto q(p)                       注意p和q指向同一对象，引用计数为2
	//	auto r = make_shared<int>(60)   r指向一个新的对象，引用计数为1
	//	r = q                           给r赋值，r指向了q的地址，就像普通指针指向另外一个地址一样，
	//	                                r原来的对象(60)引用计数-1=0,释放内存
	//	                                q被r再次引用，对象(40)引用计数+1=3
	//
	//当智能指针离开作用域时调用析构函数，析沟函数递减引用计数，为0时销毁内存
	//注意智能指针只能用于动态对象,内存释放的关键是析构和引用计数，因此当容器中的智能指针指向的内存不用时应该erase
	
	//使用vector保存输入文件副本，记录每一行
	//任何没有unordered前缀的容器几乎都是有序容器
	//vector是有序容器，是按照存入顺序的有序
	//vector存储的数据类型必须相同
	std::shared_ptr<std::vector<std::string>> file;              
	//map key代表查询的单词，value是一个set保存单词出现的行号集合
	//map和set都是有序容器，基于关键字有序
	//没有multi前缀的几乎都是关键字不可重复的,map和set是关键字不可重复的容器
	//使用insert像map或set插入元素，如果关键字已存在则什么也不做，insert返回false
	std::map<std::string,std::shared_ptr<std::set<line_no>>> wm;
	static std::string cleanup_str(const std::string &);
};

#endif


//==================================================附录=================================================
//智能指针
//	作用：简化指针的使用，避免忘记delete或程序异常catch时忘记释放内存造成内存泄露
//	原理：利用RALL（资源获取即初始化）的技术对普通指针进行封装，智能指针实质是一个对象，在析构时对指针进行释放。
//	注意：智能指针只能用来保存堆上分配的内存地址！
//	link: https://www.cnblogs.com/wxquare/p/4759020.html
//	
//	shared_ptr: 多个指针指向相同的对象，使用引用计数，初始化和拷贝构造引用计数+1，赋值和析构引用计数-1，当为0时删除指向的堆内存
//		注意：不要用同一个原始指针初始化多个shared_ptr,会造成二次释放同一内存
//		注意：避免循环引用，会造成内存无法释放。(weak_ptr可以解决这个问题,即A和B内部都只持有对方的weak_ptr)
//		注意：shared_ptr析构时默认调用delete,所以像std::shared_ptr<int> p(new int[12])会造成内存泄露，需要自定义析构
//		void deleter(int *x){delete[] x;} std::shared_ptr<int> p(new int[12],deleter);
//		线程安全性：1.多线程调用不同shared_ptr对象成员函数安全；2.多线程同时读同一个shared_ptr对象安全；3.多线程对同一个shared_ptr对象进行读和写不安全。
//		int a = 10;
//		std::shared_ptr<int> pa = std::make_shared<int>(a);//引用计数+1
//		std::shared_ptr<int> pb(pa);  //copy,引用计数+1,pa.use_count() = pb.use_count() = 2;
//		int b = 20;
//		std::shared_ptr<int> px = std::make_shared<int>(b);
//		pb = px;//引用计数-1,pa.use_count() = 1; pb.use_count() = px.use_count() = 2;
//	
//	unique_ptr: 同一时刻只能有一个unique_ptr指向给定对象（通过禁止构造拷贝、只有移动语义来实现）
//		指向对象的析构时机：智能指针析构时。1.调用reset() 2.调用reset(new xxx()),旧对象被析构 3.赋值nullptr
//		注意：调用release()只是放弃内部对象的所有权并不会释放内存。
//		注意：unique_ptr对于数组的管理可以直接这样，unique_ptr<int[]> p(new int[12]),这时析构会调用delete[],不会造成内存泄露
//		int main() {
//		    {
//		        std::unique_ptr<int> uptr(new int(10));  //绑定动态对象
//		        //std::unique_ptr<int> uptr2 = uptr;  //不能賦值
//		        //std::unique_ptr<int> uptr2(uptr);  //不能拷貝
//		        std::unique_ptr<int> uptr2 = std::move(uptr); //轉換所有權
//		        uptr2.release(); //释放所有权
//		    }
//		}
//	
//	weak_ptr: 为协助shared_prt工作引入的一种智能指针，没有重载operator*和->,仅作为观测者观测资源使用。
//		weak_ptr可以从一个shared_ptr或weak_ptr对象构造，但是不会引起引用计数+1
//		expire()等价于use_count() == 0
//		lock()可以获取一个可用的shared_ptr,从而操作资源，但当expire() = true时返回一个储存空指针的shared_ptr.
//		int main() {
//		        std::shared_ptr<int> sh_ptr = std::make_shared<int>(10);
//		        std::weak_ptr<int> wp(sh_ptr);
//		
//		        std::cout << "weak_ptr: " <<  wp.use_count() << std::endl;  //1
//		        std::cout << "sh_ptr: " <<  sh_ptr.use_count() << std::endl;//1
//		
//		        if(!wp.expired()){
//				std::shared_ptr<int> sh_ptr2 = wp.lock(); //get another shared_ptr
//			    	*sh_ptr = 100;
//		
//				std::cout << "weak_ptr: " <<  wp.use_count() << std::endl;    //2
//				std::cout << "sh_ptr: " <<  sh_ptr.use_count() << std::endl;  //2
//				std::cout << "sh_ptr2: " <<  sh_ptr2.use_count() << std::endl;//2
//			}
//		}
