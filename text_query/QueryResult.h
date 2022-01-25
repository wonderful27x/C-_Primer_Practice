#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <iostream>

//文本查询结果封装类
class QueryResult {
	//输出打印设置为友元，需要访问私有数据
	friend std::ostream &print(std::ostream &, const QueryResult &);
public:
	//自定义类型
	typedef std::vector<std::string>::size_type line_no;
	typedef std::set<line_no>::const_iterator line_t;
	QueryResult(std::string s,
		    std::shared<std::set<line_no>> p,
		    std::shared<std::vector<std::string>> f) : 
		sought(s), lines(p), file(f) {}
	//返回匹配的行数
	//注意返回的时set的size，所以类型是set<>::size_type
	std::set<line_no>::size_type size() const { return lines->size(); }
	//返回set的前向迭代器和尾后迭代器，
	//前向迭代器指向第一个元素，尾后迭代器指向最后一个元素之后的位置，不能解引用
	//需要注意的是如果容器为空，begin返回尾后迭代器，c前缀表示返回const的迭代器
	line_it begin() const { return lines->cbegin(); }
	line_it end() const { return lines->cend(); }
	//获取输入的文件
	std::shared_ptr<std::vector<std::string>> get_file() { return file; }

private:
	std::string sought;                             //查询的关键字
	std::shared_ptr<std::set<line_no>> lines;       //关键字搜索结果行号集合
	std::shared_ptr<std::vector<std::string>> file; //指向输入文件的智能指针
};

#endif
