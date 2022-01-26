#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>

#include "Query.h"
#include "TextQuery.h"

using std::shared_ptr;
using std::make_shared;
using std::set;
using std::set_intersection;
using std::ostream;
using std::size_t;
using std::inserter;

//取反查询，就是查询某个单词未出现的行号
QueryResult NotQuery::eval(const TextQuery &text) const {
	auto result = query.eval(text);
	//make_shared在动态内存中分配一个对象并初始化它，然后返回此对象的shared_ptr
	//下面的ret_lines指向一个值初始化的set<line_no>,空的set
	auto ret_lines = make_shared<set<line_no>>();
	auto beg = result.begin(), end = result.end();
	auto sz = result.ret_file()->size();
	//循环文本每一个行号，判断查询结果中出现
	for(size_t n = 0; n != sz; ++n) {
		//如果不相等，或result已经循环完了，
		//将行号记录到set
		//这里的逻辑需要思考思考，注意行号升序排列
		if(beg == end || *beg != n)
			ret_lines->insert(n);
		//否则++result行号
		else if(beg != end)
			++beg;
	}
	
	//返回最终结果
	return QueryResult(rep(), ret_lines, result,get_file());
}

//&查询操作，两个查询对象结果的交集
QueryResult AndQuery::eval(const TextQuery &text) const {
	auto left = lhs.eval(text), right = rhs.eval(text);
	auto ret_lines = make_shared<set<line_no>>();
	//算法将两个输入序列中共同出现的元素写到目的位置中
	//目的位置是一个用inserter构造的插入迭代器
	
	//注意算法本身永远不会执行容器的操作，算法运行在迭代器之上，执行迭代器的操作
	//算法永远不会改变底层容器的大小，因此永远不会直接添加或删除元素
	//但可以改变元素的值，也可以在容器中移动元素，
	//普通迭代器一般可以访问元素或修改元素的值，但是没有插入删除的操作，
	//因此在下面的算法中我们必须使用inserter构造一个特殊的插入迭代器，
	//这样算法才能通过插入迭代器间接插入元素
	set_intersection(left.begin(), left.end(),
			rigth.begin(), right.end(),
			inserter(*ret_lines, ret_lines->begin()));

	return QueryResult(rep(), ret_lines, left.get_file());
}

//|查询操作，将两个Query查询结果合并
QueryResult OrQuery::eval(const TextQuery &text) const {
	auto left = lhs.eval(text), right = rhs.eval(text);
	auto ret_lines = make_shared<set<line_no>>(left.begin(), left.end());
	ret_lines->insert(right.begin(), right.end());

	return QueryResult(rep(), ret_lines, left.get_file());
}
