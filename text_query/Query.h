//====================文本查询程序-接口设计========================
//该程序涵盖的知识点：
//一.OOP面向对象程序设计
//	构造函数(执行流程、成员初始化顺序、子类父类构造函数调用先后顺序)
//	合成版本的拷贝构造函数和拷贝赋值元算符
//	普通类的名字查找
//	继承体系名字查找-函数调用解析过程
//	析构函数(执行流程、成员销毁顺序、子类父类析构函数调用先后顺序)
//	析构函数调用时机
//	虚函数
//	纯虚函数
//	虚函数属性被继承
//	动态绑定
//	抽象类
//	访问控制与继承
//	
//
//二.容器
//	顺序容器vector
//	关联容器map，set
//	有序容器,对关键字的比较要求
//	无序容器
//	可重复关键字/不可重复关键字
//	map的find和下标运算的区别
//	迭代器，迭代器范围
//	关联容器的关键数据成员key_type,mapped_type,value_type
//	pair
//	算法，算法永远不执行容器操作
//
//
//三.动态内存
//	智能指针shared_ptr,make_shared
//	智能指针也是模板
//	引用计数
//	智能指针智能用于动态内存
//
//
//四.其他
//	局部静态对象
//	auto,推演
//	范围for循环



//文本查询应用的继承体系
//在文本查询中TextQuery拥有查询某个单词的能力，但是查询还需要&、|、~等操作，
//它们通常是对某个单词的查询结果的交并集和取反，因此像这样的操作并不适合继承
//TextQuery,而是使用TextQuery,我们将自己封装一套继承体系
//				Query_base
//	    _________________________|____________________
//         |                         |                    |
//	WordQuery                NotQuery             BinaryQuery  
//	                                      ____________|_______
//                                           |                    |
//	                                   AndQuery             OrQuery
//Query_base提供统一查询接口，抽象基类
//WordQuery实现单个单词查询
//NotQuery实现~查询
//BinaryQuery是一个拥有两个查询对象的抽象基类
//AndQuery实现&查询
//OrQuery实现|查询
//
//注意上面是查询操作程序设计的继承体系，对于查询用户我们应该隐藏这些复杂的细节
//因此我们提供一个用户层面的接口Query,是查询程序的使用变得简单
//一个复杂的符合查询设计成这样:
//(查询fiery和bird在一行同时出现，或者一行出现了wind的结果)
//Query q = Query("fiery") & Query("bird") | Query("wind");
//Query于继承体系的关系图:
//                                             |___  WordQuery -> fiery
//                             |___ AndQuery --|___  WordQuery -> bird
//	q = Query -- OrQuery --|
//                             |___ WordQuery -> wind
//
//
// * &运算符生成一个绑定到新的AndQuery对象上的Query对象
// * |运算符生成一个绑定到新的OrQuery对象上的Query对象
// * ~运算符返回一个绑定到新的NotQuery对象上的Query对象


#ifndef QUERY_H
#define QUERY_H

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include "TextQuey.h"

//virtual虚函数
//虚函数是实现动态绑定的关键，当调用基类指针或引用的虚函数时，将调用其真实派生类的版本
//动态绑定只有通过指针或引用调用虚函数时才会发生
//一个函数声明为虚函数，其派生类中隐式地也是虚函数
//如果成员函数是非虚函数，解析过程发生在编译时
//普通函数不使用时可以不提供定义，但是虚函数必须提供定义

//名字查找与继承-函数调用的解析过程
//假设Query_base *q = new NotQuery(); q->eval(file);
//1.首先确定q的静态类型，静态类型为Query_base
//2.在q的静态类型中查找eval,如果找不到，则依次在直接基类中不断查找直到到达继承链顶端，找不到编译器报错
//3.一旦找到了，就进行常规类型检查以确认对于当前的调用是否合法，如果不合法编译器报错
//4.如果合法
//	* 如果eval是虚函数其是通过引用或指针进行调用，则依据对象的动态类型进行调用
//	* 如果eval不是虚函数或是通过对象调用，编译器产生一个常规函数调用(静态类型的函数调用)

//抽象基类，提供eval和rep接口
//纯虚函数:在虚函数声明后写=0,纯虚函数的函数体必须在类外定义
//含有(或未覆盖直接继承)纯虚函数的类是抽象基类，抽象基类不能创建对象
class Query_base {
	//友元声明并非真正意义上的声明，所以Query不必在之前出现
	friend class Query;
//访问控制
//public: 公有的，类的用户、继承者都可访问
//protected: 受保护的，类的用户不能访问，继承者可以访问
//private: 只有类自己的成员可以访问
protected:
	using line_no = TextQuery::line_no;
	//!!!=======继承体系中析构函数(析构函数执行流程，成员销毁顺序，析构函数调用顺序)=======
	//析构函数有着和构造函数相反的行为
	//构造函数用于创建和初始化资源，先初始化再执行函数体,初始化顺序按照类中定义的顺序
	//析构函数用于释放资源，先执行函数体再隐式销毁成员(指针类型不会调用delete),
	//销毁顺序按照初始化顺序逆序执行,类类型将调用其析构函数，内置类型什么也不做
	//派生类析构函数只负责销毁派生类自己分配的资源
	//派生类析构函数首先调用，然后是基类析构函数，沿着继承体系反方向类推
	
	//对象被销毁析构函数被调用，析构函数调用时机:
	//1.变量离开作用域
	//2.一个对象被销毁时，其成员被销毁
	//3.容器被销毁时，其元素被销毁
	//4.动态分配对象指针调用delete
	//5.创建临时对象的完整表达式结束时

	//继承体系中通常将析构函数声明为虚析构函数以便析构是执行动态绑定，得以正确释放资源
	//根据上面的描述，派生类析构函数也将是虚函数
	virtual ~Query_base() = default;

//接口设计为private，全部交给友元Query类来对外提供
private:
	//使用TextQuery进行查询，返回查询结果
	virtual QueryResult eval(const TextQuery &) const = 0;
	//返回基础查询的string形式,比如WordQuery返回的就是要查询的单词wind
	//AndQuery返回的就是形如: fiery&bird
	//整个Query返回的是: fiery&bird|wind
	virtual std::string rep() const = 0;
};


//Query是真正为用户使用提供的接口，它主要定义了eval和rep接口，
//内部持有Query_base的指针，因为需要动态绑定所以是指针，而我们使用智能指针来管理
//真正的查询工作交给Query_base的派生类来完成
class Query {
	friend Query operator~(const Query &);
	friend Query operator|(const Query &, const Query &);
	friend Query operator&(const Query &, const Query &);
public:
	//string参数的构造函数用于创建单个查询的对象，即WordQuery
	Query(const string &);
	QueryResult eval(const TextQuery &t) const {
		return q->eval(t);
	}
	std::string rep() const { return q->rep(); }
private:
	//Query_base为参数的构造函数声明有private,
	//因为我们不希望用户使用，我们将使用~、|、&重载运算符函数来创建
	Query(std::shared_ptr<Query_base> query) : q(query) {}
	//持有Query_base的指针，查询工作的任务将转接给它
	std::shared_ptr<Query_base> q;
};


//输出重载运算符，非成员函数
//因为要使用Query所以必须在其后定义
inline std::ostream &operator<<(std::ostream &os, Query &query) {
	return os << query.rep();
}


//访问控制与继承
//派生访问说明符的目的是控制派生来的用户以及派生类的派生类对基类成员的访问权限
//而派生类对基类成员的访问规则不变,应该特别理解这两句话
//public: 公有继承，继承的成员在派生类中的访问修饰符与基类一致
//protected: 受保护继承，基类public成员在派生类变成protected,这意味着派生类用户无法访问了，
//相当于在继承中修改了对外的访问权限
//private: 私有继承，都变成private的
//对于class默认是private继承
//这里还有一条规定，不太好理解，page 543
//!!!派生类成员或友元，可以通过派生类对象来访问基类成员的protected成员，
//对于基类对象protected成员没有特殊访问权限,注意这里强调的是对象

//WordQuery自己的成员都是私有的，因为我们不希望用户直接使用，而是通过Query类来间接操作
class WordQuery : public Query_base {
	friend class Query;
	//调用TextQuery查询
	//使用override关键字显示注明它将覆盖基类虚函数
	//使用override能在编译时帮我们找出一些错误,如参数列表或函数名不小心写错了，或不是virtual虚函数
	QueryResult eval(const TextQuery &t) const override {
		return t.query(query_word);
	}
	//返回查询关键字
	std::string rep() const override {
		return query_word;
	}
	std::string query_word;
};

//派生类
//派生类对象包含多个组成部分:
//派生类自己定义的非静态成员子对象
//继承的基类的子对象，如果有多个基类，则也会有多个这样的子对象
//!!!=========继承体系中构造函数(构造函数执行流程，成员初始化顺序，构造函数调用顺序)=======
//每个类控制自己成员的初始化，派生类控制自己的，
//通过调用基类构造函数让基类控制自己的成员初始化
//继承体系中首先初始化基类的部分，然后按照声明顺序依次初始化派生类的成员

//因为要用到WordQuery，所以到这里才定义Query构造函数
inline Query::Query(const std::string &s) : q(new WordQuery(s)) {}

//取反查询，即查询某个单词没出现的行号
class NotQuery : public Query_base {
	//取反运算符，通过一个Query构造并返回一个NotQuery,
	//因为要使用其私有构造函数，所以需要声明为友元
	friend Query operator~(const Query &);
	NotQuery(const Query &q) : query(q) {}
	std::string rep() const override { return "~(" + query.rep() + ")"; }
	QueryResult eval(const TextQuery &) const override;
	//持有一个Query,利用query的封，简化程序
	//前面提到Query是用户接口，当然我们自己也可以使用
	//构造的时候通常就是一个WordQuery
	//还有一个原因就是operator~操作
	Query query;
};


//拥有两个查询操作的query，它是一个抽象基类，
//作为&和|查询操作的父类
class BinaryQuery : public Query_base {
protected:
	//注意参数Query &l, &r传递的是引用，但是lhs和rhs本身是普通变量
	//在执行初始值列表的初始化过程当中实际是通过Query的合成版本的拷贝构造函数来完成的
	//Query中的Query_base q是智能指针，所以内部自动执行引用计数的+/-，因此我们无需担心
	BinaryQuery(const Query &l, const Query &r, std::string s) : 
		lhs(l), rhs(r), opSym(s) {}
	//覆盖rep函数，让派生类覆盖eval函数
	std::string rep() const override {
		return "(" + lhs.rep() + " " + opSym + " " + rhs.rep()  + ")"; 
	}
	//持有两个查询Query对象
	Query lhs, rhs;
	//操作符，&或|
	std::string opSym;
};


//&查询操作，继承BinaryQuery,是两个Query查询结果的交集
class AndQuery : public BinaryQuery {
	friend Query operator&(const Query &, const Query &);
	AndQuery(const Query &left, const Query &right) : 
		BinaryQuery(left, right, "&") {}
	QueryResult eval(const TextQuery &) const override;
};


//|查询操作，继承BinaryQuery,是两个Query查询结果的并集
class OrQuery : public BinaryQuery {
	//友元函数，|将通过两个Query对象构造并返回OrQuery
	friend Query operator|(const Query &, const Query &);
	OrQuery(const Query &left, const Query &right) : 
			BinaryQuery(left, right, "|") {}
	QueryResult eval(const TextQuery &) const override;
};

//&重载运算符，是一个二元运算符，通过两个Query对象构造一个AndQuery
inline Query operator&(const Query &lhs, const Query &rhs) {
	//这里有两个需要理解的点
	//一是用于构造AndQuery的两个Query对象最终作为BinaryQuery中两个成员Query lhs,rhs的拷贝构造函数参数,
	//最终构造出了BinaryQuery中的lhs和rhs成员
	//二是return等价于
	//shared_ptr<Query_base> tmp(new AndQuery(lhs, rhs);
	//return Query(tmp);
	//这实际上就是隐式转换，构造函数Query需要一个shared_ptr<Query_base>的参数，
	//我们直接返回了它的一个参数，这使得Query的构造函数被隐式调用
	return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}

inline Query operator|(const Query &lhs, const Query &rhs) {
	return std::shared_ptr<Query_base>(new OrQuery(lhs, rhs));
}

inline Query operator~(const Query &operand) {
	return std::shared_ptr<Query_base>(new NotQuery(operand));
}

//其他用户接口
std::ifstream &open_file(std::ifstream &, const std::string &);
TextQuery get_file(int, char**);
bool get_word(std::string &);
bool get_words(std::string &, std::string &);
std::ostream &print(std::ostream &, const QueryResult &);

//剩余比较复杂的eval函数定义在Query.cpp中

#endif
