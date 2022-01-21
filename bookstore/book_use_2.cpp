#include <iostream>

#include "Book_sale.h"
#include "bookexcept.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char **argv){
	Book_sale trans,total;
	cout << "请输入销售记录，格式： 书本编号(string) 销售数量(unsigned) 单价(double), ctrl+d 退出" << endl;
	//读到第一条记录,进入if流程
	if(cin >> total) {
		//while循环读取剩下的记录
		while(cin >> trans) {
			try {
				//累加同编号的书到total
				total += trans;
			}catch(const isbn_mismatch &e) {
				//如果编号不同就会抛出异常
				//这时打印total,并让total等于新的编号以便下一轮累加
				cout << total << endl;
				total = trans;
			}
		}
		//while结束后打印最后的total
		cout << total << endl;
	}
	//如果一条记录都没有，通知用户
	else {
		cerr << "No data?!" << endl;
	}

	return 0;
}
