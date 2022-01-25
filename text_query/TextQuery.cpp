#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <utility>

#include "TextQuery.h"
#include "make_plural.h"

//using 声明，引入单个成员
//注意上面的像<cstring>是c语言<string.h>的c++版本，
//区别在于<cstring>定义在std命名空间,c++程序使用c库时都应该这么做
using std::size_t;
using std::shared_ptr;
using std::istringstream;
using std::string;
using std::getline;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::cout;
using std::cin;
using std::ostream;
using std::endl;
using std::ifstream;
using std::ispunct;
using std::tolower;
using std::strlen;
using std::pair;

//构造函数，将一个输入文件的每一行保存到vector作为一个副本
//并记录每个单词出现的行号set集合
//构造函数首先使用初始值列表初始化file，shared_ptr必须指向动态内存，所有必须是new
//指针指向一个空的vector<string>,引用计数为1
TextQuery::TextQuery(ifstream &is) : file(new vector<string>) {
	string text;
	//getline函数从文件输入流读取一行数据保存到text
	while (getline(is, text)) {
		//将数据保存到vector的尾端
		file->push_back(text);
		int n = file->size() - 1;
		//istringstream是一个string流，头文件<sstring>,它使string能像一个流来处理
		//istringstream是输入流，从string中读取数据
		istringstream line(text);
		string word;
		//循环从一行文本中逐个读取每个单词
		while (line >> word) {
			//移除标点符号，并转换为小写，以便查询不区分大小写
			word = cleanup_str(word);
			//map下标操作！！！
			//下标运算符[]接收一个关键字,获取关键字关联的值，如果关键字不在map中，
			//则会创建这样一个key-value插入map中，value进行值初始化
			//因为下标操作可能插入元素，所以下标操作只能对非const map使用
			//如果value是内置类型则为0,否则由类默认初始化
			//假设有个map<string,int> mp,string key = "age",执行mp[key] = 10 流程如下
			//1.mp中搜索关键字age,没有找到
			//2.将创建一个新的key-value键值对age-0插入mp，key是const string
			//3.提取出新插入的元素，将10赋值给它
			//因此当对一个新关键字第一次使用wm[word]时，会插入一个shared_ptr的value,
			//因为是类类型，执行默认初始化，shared_ptr默认初始化时保存一个空指针
			auto &lines = wm[word];
			//所以第一下标操作的line保存空指针，
			//reset让其指向新的对象,此时创建了保存行号的set
			if(!line) lines.reset(new set<line_no>);
			//单词所在的行号插入set
			//因为set是关键字不重复，且对关键字有序的容器
			//当同一行有多个相同的单词时多次insert相同的关键字不会其作用
			//并且能保证升序排列
			//对于有序容器，关键字类型必须定义比较方法，默认情况下，
			//标准库使用关键字类型<运算符比较，所以默认是升序号
			lines->insert(n);
		}
	}
	//当整个对象构造完成，vector<string> file 按顺序保存了文本的每一行内容
	//map<string,set<line_no> wm 保存了每个单词出现的行号集合
}
	
//移除标点符号，并将所有大写转换为小写
//注意istringstream默认空格隔开的为一个输入，
//如输入hello world, ok,haha得到如下单词：
//hello
//world,
//ok,haha
string TextQuery::cleanup_str(const string &word) {
	string ret;
	for(auto it=word.begin(); it!=word.end(); ++i) {
		//如果不是标点符号则转换为小写
		if(!ispunct(*it))
			ret += tolower(*it);
	}

	return ret;
}

//查找一个单词在文本中出现的行号
QueryResult TextQuery::query(const string &sought) {
	//如果没有找到，返回一个空的
	//局部静态对象在程序执行第一次经过其定义时初始化，直到程序终止才销毁
	static shared_ptr<set<line_no>> nodata(new set<line>);
	//auto（page 62,推演类型并不简单，需要多看书研究）
	//1.auto让编译其通过初始值来推算变量的类型，在复杂类型或不知道是什么类型时很有用
	//在这里是一个迭代器类型: map<string,shared_ptr<set<line_no>>::iterator
	//2.int i=0, &r=i; auto a = r
	//编译器以引用对象的类型作为auto的类型，auto是一个整数类型而非引用类型
	//3.auto一般忽略顶层const，保留底层cosnt
	//int i=0; const int ci=i, &cr=ci;
	//auto b = ci; //b是整数类型，ci是顶层cosnt
	//auto c = cr; //c是整数类型，cr是ci别名，ci是顶层cosnt,疑问: 理论上来说cr本身是一个底层const，为啥这里？？？
	//auto d = &i; //d是整形指针
	//auto e = &ci;//e是指向整数常量的指针，对常量对象取地址是底层const
	//希望推演出顶层cosnt
	//const auto f = ci; //ci推演类型是int，f是const int
	//希望将引用类型设置为auto
	//auto &g = ci; //g是整形常量引用
	
	//迭代器(迭代器为我们提供访问容器的公共接口，不同容器根据特性有不同的实现)
	//迭代器的基本作用就是遍历，所有容器迭代器都定义了递增运算符，从当前元素移动到下一元素
	//常用迭代器类型有:
	//const_iterator:只能读元素，不能写元素
	//iterator:对元素可读也可写
	//如果容器元素是const只能使用const_iterator,否则二者都可以使用
	//迭代器范围由一对迭代器(通常称为begin和end)组成，表示容器元素的范围,两个迭代器必须指向同一容器中的元素
	//begin为前向迭代器指向范围的第一个元素，end为尾后迭代器指向范围最后一个元素之后的位置,他们组成左闭合区间[begin,end)
	//一个容器r，r.begin()得到一个指向容器第一个元素的迭代器，r.end()得到一个指向容器最后一个元素之后位置的迭代器
	//加上前缀r.cbegin()返回const_iterator类型迭代器，不加前缀r.begin(),返回类型依赖容器元素类型
	//begin == end 范围为空
	//begin != end 范围至少有一个元素
	
	//find返回一个迭代器，指向第一个(对于可重复关键字的容器可能有多个结果)关键字为key的元素，
	//如果没有找到则返回尾后迭代器
	//这里不能用下标运算符号，因为下标运算符在容器没有此关键字时会创建一个然后插入容器
	auto loc = wm.find(cleanupf_str(sought));
	//如果是尾后迭代器说明没有找到，返回一个空set的result
	if(loc == wm.end())
		return QueryResult(sought, nodata, file);
	else
		//pair是标准库类型，它保存两个成员，分别是first和second
		//对于关联容器
		//key_type: 关键字类型
		//mapped_type: 关键字关联的类型，只适用于map！！！
		//value_type: 对于set和key_type相同，对于map为pair<const key_type,mapped_type>
		//迭代器类似指针，解引用关联容器迭代器得到的是value_type,如map是pair<const key_type,mapped_type>
		//因为const key_type的缘故，map无法改变关键字的值，只能改变mapped_type的值，如++map_it->second
		//set容器的迭代器(iterator和const_iterator)是const的，set的关键字也是const的，因此set只能读取关键字不能改变关键字的值
		return QueryResult(sought, loc->second, file);
}

//打印查询结果：查询单词以及行号和对应的文本
ostream &print(ostream &os, const QueryResult &qr) {
	os << qr.sought << " occurs " << qr.lines->size() << " "
	   << make_plural(qr.lines->size(), "time", "s") << endl;
	//范围for循环，:右边是一个序列，左边类型必须是右侧序列元素能转换的类型，代表一个序列元素
	//每次迭代会重新定义变量num并将其初始化为:右侧序列的下一个元素
	for(auto num : *qr.lines)
		//输出行号
		os << "\t(line " << num + 1 << ") "
		   //输出行号对应的文体内容，迭代器加上一个数字指向偏移之后位置的元素
		   //下面表达式迭代器将指向vector第num个位置的元素
		   << *(qr.file->begin() + num) << endl;

	return os;
}

//debugging routine
void TextQuery::display_map() {
	//注意wm类型：
	//std::map<std::string,std::shared_ptr<std::set<line_no>>> wm;
	auto iter = wm.cbegin(), iter_end = wm.cend();
	for( ; iter != iter_end; ++iter) {
		//打印单词
		cout << "word: " << iter->first << " {";
		auto text_locs = iter->second;
		//单词行号set的首尾迭代器
		auto loc_iter = text_locs->cbegin(), 
		     		loc_iter_end = text_locs->cend();
   		//遍历行号set集合
		while(loc_iter != loc_iter_end) {
			//输出行号
			cout << *loc_iter;
			if(++loc_iter != loc_iter_end)
				cout << ", ";
		}
		cout << "}\n";
	}
	cout << endl;
}
