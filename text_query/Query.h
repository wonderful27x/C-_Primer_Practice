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
//      拷贝控制成员的作用
//      三五法则，什么时候应该定义拷贝控制成员
//      拷贝成员的合成版本规则(普通类和继承体系)
//      被删除的合成拷贝成员规则(普通类和继承体系)
//      移动成员的合成版本规则(普通类和继承体系)
//      被删除的合成移动成员规则(普通类和继承体系)
//      析构函数组织移动操作规则(普通类和继承体系)
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
//	智能指针shared_ptr,make_shared,weak_ptr
//	智能指针注意事项
//	智能指针也是模板
//	引用计数
//	智能指针智能用于动态内存
//	new、malloc和allocator
//
//
//四.其他
//	局部静态对象
//	auto,推演
//	范围for循环
//	lambda表达式和函数对象



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

#include "TextQuery.h"

//virtual虚函数
//虚函数是实现动态绑定的关键，当调用基类指针或引用的虚函数时，将调用其真实派生类的版本
//动态绑定只有通过指针或引用调用虚函数时才会发生
//一个函数声明为虚函数，其派生类中隐式地也是虚函数
//如果成员函数是非虚函数，解析过程发生在编译时
//普通函数不使用时可以不提供定义，但是虚函数必须提供定义
//
//多态实现原理(下面的描述针对有virtual关键字的情况)
//	* 编译器会为有虚函数的类生成虚表
//	* 基类和派生类都有属于自己独立的虚表，虚表是一维数组，存放者对应类的虚函数
//	* 每个对象有一个虚表指针，构造函数中将其指向自己的虚表
//	* 调用函数时先判断对象的具体类型，调用其虚表指针对应的函数

//普通类对象的名字查找(page 254)
//	* 类的定义分两步:
//		* 首先编译成员的声明
//		* 直到类全部可见后才编译函数体
//	* 用于类成员声明中的名字查找
//		* 声明中使用的类型名，如成员类型、函数返回类型或参数类型的名字都必须在使用前可见，
//		* 如果使用的类型名Typename在类的内部使用之前的域没有定义，则在定义类的作用域类定义之前的域继续查找
//	* 成员定义中的普通块作用域的名字查找
//		* 在函数体内使用名字之前的域查找
//		* 如果没有找到，在类内找，类的全部成员都被考虑
//		* 类内也没有找到，在函数定义之前的作用域找

//函数名字查找与继承-函数调用的解析过程
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
	Query(const std::string &);
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
	WordQuery(const std::string &s) : query_word(s) {}
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



//=============================================附录==================================================
//auto: 由编译器推断表达式所属的类型
//	* 作用: 将表达式的值赋值给一个变量，但不清楚表达式的类型
//	* 引用类型auto推断为引用的对象的类型
//		int i = 0, &r = i;
//		auto a = r;  //a是一个整数
//	* auto通常忽略顶层const保留底层const
//		const int ci = i, &cr = ci;
//		auto c = cr; //a是一个整数，cr是ci的别名，而ci是一个顶层const
//
//decltype: 选择并返回操作数的类型，编译器分析表达式类型并不实际计算表达式
//	作用: 用表达式推断出的类型定义自己的变量，但不想用表达式的值初始化
//	* 如果表达式是一个变量，返回变量类型，包括顶层const和引用类型
//		cosnt int ci = 0, &cj = ci;
//		decltype(ci) x = 0; //x是cosnt int
//		decltype(cj) y = x; //y是const int &, y绑定到x
//	* 转变引用类型
//		int i = 42, &r = i;
//		decltype(r+0) b; //b是int
//	* 表达式是解引用，得到引用类型
//		int i = 42, *p = i;
//		decltype(*p) x = i; //x是int &
//	* 表达式是加上括号的变量，得到引用类型
//		int i = 42;
//		decltype((i)) d; //错误，d是int &, 必须初始化
//
//
//容器迭代器与算法
//	* 容器操作: 注意容器是模板，每个实例都有独属它自己的内部类型,如iterator
//		* 成员
//			* iterator: 此容器迭代器类型
//			* cosnt_iterator: 元素只读迭代器
//			* size_type: 无符号整型，足够保存容器最大可能容量大小
//			* difference_type: 带符号整型，足够保存两个迭代器之间的距离
//			* value_type: 元素类型
//		* 方法
//			* size(): 元素数目（forward_list不支持）
//			* max_size(): 可保存的最大元素数目
//			* empty(): 是否存储了元素
//			* insert(args): 将args中的元素拷贝到容器
//			* emplace(inits): 使用inits构造容器的一个元素，注意会调用构造函数而不是拷贝元素
//			* erase(args): 删除args指定元素
//			* clear(): 删除所有元素
//		* 运算
//			* ==,!=: 所有容器都支持相等和不相等运算
//			* <, <=, >, >=: 关系运算，无序关联容器不支持
//	* 迭代器
//		* 作用
//			* 提供访问容器的公共接口，不同容器根据特性有不同的实现
//			* 迭代器的基本作用就是遍历，所有容器迭代器都定义了递增运算符，从当前元素移动到下一元素
//		* 迭代器范围(左闭右开规则,两迭代器需指向同一容器): 
//			* begin为前向迭代器指向范围的第一个元素，
//			* end为尾后迭代器指向范围最后一个元素之后的位置,不能解引用
//			* begin == end 范围为空
//			* begin != end 范围至少有一个元素
//		* 两类迭代器
//			* const_iterator:只能读元素，不能写元素
//			* iterator:对元素可读也可写
//			* 如果容器元素是const只能使用const_iterator,否则二者都可以使用
//			* 加上前缀r.cbegin()返回const_iterator类型迭代器，不加前缀r.begin(),返回类型依赖容器元素类型
//		* 注意!!!
//			* 添加删除元素操作可能使指向容器的指针、引用、或迭代器失效，失效将不再表示任何元素
//			* 所以有添加删除操作后应该重新获取迭代器
//	* 顺序容器: 这种顺序不依赖于元素的值，而是指元素加入容器时的位置
//		* 常用容器
//			* vector: 可变大小数组，随机访问，尾部之外插入或删除元素可能很慢
//			* deque: 双端队列，随机访问，头尾插入删除元素很快
//			* list: 双向链表，只支持双向顺序访问，任何位置插入删除都很快
//			* forward_list: 单项链表，只支持单向顺序访问，任何位置插入删除都很快
//			* array: 固定大小数组，随机访问，不能添加和删除元素
//			* string: 于vector相似
//		* 添加元素
//			* push_back(t): 尾部插入元素,forward_list不支持
//			* empplace_back(args): 尾部构建元素,forward_list不支持
//			* push_front(t): 头部插入元素,vector/string不支持
//			* emplace_front(args): 头部构建元素,vector/string不支持
//	* 注意:
//		* 用对象初始化容器，或将一个对象插入容器时，放入的是元素的拷贝	
//	* 适配器: 标准库通用概念，使一种事物的行为看起来像两一种事物的一种机制
//		* 容器、迭代器、函数都有适配器
//		* 如插入迭代器就是一种容器适配器
//	* 算法
//		* 注意算法本身永远不会执行容器的操作，算法运行在迭代器之上，执行迭代器的操作
//		* 算法永远不会改变底层容器的大小，因此永远不会直接添加或删除元素
//		* 但可以改变元素的值，也可以在容器中移动元素，
//		* 普通迭代器一般可以访问元素或修改元素的值，但是没有插入删除的操作，但是可以构造如插入迭代器来让算法可以向容器插入元素
//	* 关联容器: 通过关键字进行存取数据，关联容器也可以是有序的(对关键字有序)
//		* 常用容器
//			* map: 关联数组，保存关键字-值对,有序,不可重复关键字
//			* set: 只保存关键字,有序，不可重复关键字
//			* multimap: 关键字可重复的map,有序
//			* multiset: 关键字不可重复的set,有序
//			* unordered_map: 用哈希函数组织的map
//			* unordered_set: 用哈希函数组织的set
//			* unordered_multimap: 用哈希函数组织的multimap
//			* unordered_multiset: 用哈希函数组织的multiset
//		* pair: 是标准库类型，它保存两个成员，分别是first和second
//		* 关联容器成员
//			* key_type: 关键字类型
//			* value_type: 对于set和key_type相同，对于map为pair<const key_type,mapped_type>
//			* mapped_type: 关键字关联的类型，只适用于map！！！
//		* 关联容器对关键字的要求
//			* 有序容器: 关键字必须定义元素比较的方法，默认标准库使用关键字类型的<运算符,所以默认是升序
//			* 无序容器: 默认使用==运算符比较元素；使用hash<key_type>模板生成每个元素的哈希值，自定义类型必须特例化hash模板，内置类型不需要标准实现了
//		* 注意: 
//			* 关联容器的关键字是const的，不能被改变
//				* 对于map,value_type是pair<const key_type,mapped_type>,可以改变pair的值，但永远不能改变关键字的值
//				* 对于set，关联的关键字也是const的
//			* insert插入元素，如果关键字已经存在，insert什么也不做，返回false
//			* map下标运算
//				* 下标运算符[]接收一个关键字,获取关键字关联的值，如果关键字不在map中，
//				* 则会创建这样一个key-value插入map中，value进行值初始化
//				* 如果value是内置类型则为0,否则由类默认初始化
//				* 因为下标操作可能插入元素，所以下标操作只能对非const map使用
//				* 假设有个map<string,int> mp,string key = "age",执行mp[key] = 10 流程如下
//				* 1.mp中搜索关键字age,没有找到
//				* 2.将创建一个新的key-value键值对age-0插入mp，key是const string
//				* 3.提取出新插入的元素，将10赋值给它
//				* 也就是说，对于上面的例子，当关键字不存在时，mp[key]操作一定会使容器有一个age-0的元素
//				* 如果仅仅是想确定容器是否有该元素，应该使用find
//lambda表达式和函数对象: 
//	* lambda表达式: [capture list] (parameter list) -> return type { function body }
//		* capture list: 捕获列表，用于捕获lambda表达式所在函数中的局部变量
//		* parameter list: 参数列表
//		* return type: 返回类型
//		* function body: 函数体
//		* 值捕获: 被捕获的变量的值在lambda创建时拷贝，而不是调用时拷贝
//		* 引用捕获: 必须保证在lambda执行时捕获的变量是存在的
//	* 函数对象: 定义了调用运算符()的类的对象，能像函数一样被调用。lambda表达式本质是一个函数对象
//
//new、malloc和allocator
//	* new 表达式: 将内存分配和对象构造组合在一起
//		1. 调用名为operator new(或operator new[])的标准库函数，分配足够大、原始、未命名的内存空间
//		2. 调用构造函数构造对象并传入初始值
//		3. 返回对象指针
//	* delete 表达式: 将对象析构和内存释放结合在一起
//		1. 执行对象析构函数
//		2. 调用名为operator delete(或operator delete[])的标准库函数释放内存空间
//	* allocator 模板: 分配一块原始的、未构造的内存，帮助我们把内存分配和对象构造分离
//		* allocator<T> a: 模板
//		* a.allocator(n): 分配原始内存
//		* a.construct(p, args): 构造对象
//		* a.destroy(p): 析构对象
//		* a.deallocate(p,n): 释放内存
//	* 定位new: page 409 729
//		* 和allocator的construct类似，定位new允许我们在一个特定、于先分配的内存地址上构造对象
//	* 注意: 
//		* 我们可以重载operator new和operator delete来控制内存分配细节，但是我们无法自定义new和delete表达式的行为
//		* operator new和operator delete是标准库普通函数，可以直接调用，他们的行为和allocator的allocate和dleallocate类似，
//		负责分配释放内存，但不会构造和销毁对象,这时对象的构造需要借助定位new，析构需要直接调用析构函数
//		* 直接调用析构函数只会销毁对象，不会释放内存，者也是它和delete的区别
//	* malloc和free: c语言中用于分配和释放内存，自定义的operator new和operator delete可以借助它们来完成
//
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
