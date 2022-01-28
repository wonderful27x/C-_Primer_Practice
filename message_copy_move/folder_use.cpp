#include <vector>
#include <string>
#include <iostream>

#include "Folder.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

int main() {
	string s1("contents1");
	string s2("contents2");
	string s3("contents3");
	string s4("contents4");
	string s5("contents5");
	string s6("contents6");

	//构造Message,Folders是空的set
	Message m1(s1);
	Message m2(s2);
	Message m3(s3);
	Message m4(s4);
	Message m5(s5);
	Message m6(s6);

	Folder f1;
	Folder f2;

	//message加入给定folder中
	m1.save(f1); m3.save(f1); m5.save(f1);
	m1.save(f2);
	m2.save(f2); m4.save(f2); m6.save(f2);

	m1.debug_print();
	f2.debug_print();

	//拷贝构造
	Message c1(m1);
	Message c2(m2), c4(m4), c6(m6);

	m1.debug_print();
	f2.debug_print();

	//拷贝赋值运算符
	m2 = m3;
	m4 = m5;
	m6 = m3;
	m1 = m5;

	m1.debug_print();
	f2.debug_print();

	//自赋值
	m2 = m2;
	m1 = m1;

	m1.debug_print();
	f2.debug_print();

	//拷贝message到vector
	vector<Message> vm;
	cout << "capacity: " << vm.capacity() << endl;
	vm.push_back(m1);

	cout << "capacity: " << vm.capacity() << endl;
	vm.push_back(m2);

	cout << "capacity: " << vm.capacity() << endl;
	vm.push_back(m3);

	cout << "capacity: " << vm.capacity() << endl;
	vm.push_back(m4);

	cout << "capacity: " << vm.capacity() << endl;
	vm.push_back(m5);

	cout << "capacity: " << vm.capacity() << endl;
	vm.push_back(m6);

	//拷贝folder到vector
	vector<Folder> vf;
	cout << "capacity: " << vf.capacity() << endl;
	vf.push_back(f1);

	cout << "capacity: " << vf.capacity() << endl;
	vf.push_back(f2);

	//拷贝构造folder，移动folder到vector
	cout << "capacity: " << vf.capacity() << endl;
	vf.push_back(Folder(f1));

	cout << "capacity: " << vf.capacity() << endl;
	vf.push_back(Folder(f2));

	cout << "capacity: " << vf.capacity() << endl;
	vf.push_back(Folder());

	//保存message到folder
	Folder f3;
	f3.save(m6);
	cout << "capacity: " << vf.capacity() << endl;
	//拷贝到vector
	vf.push_back(f3);

	return 0;
}


