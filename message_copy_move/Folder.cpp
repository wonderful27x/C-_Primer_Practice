#include <utility>
#include <iostream>
#include <set>

#include "Folder.h"

using std::cerr;
using std::endl;
using std::set;
using std::string;

//关于自己版本的swap和标准库的swap，以及swap是如何避免拷贝的，参考page 457
//如果使用标准库的swap大概就是
//Message tmp = lhs;
//lhs = rhs;
//rhs = tmp;
//这样相当于lhs经历了两次拷贝
void swap(Message &lhs, Message &rhs) {
	//using声明(这里非必须，但是好习惯)，将std::swap加入当前作用域page 709
	//using声明引入的与当前作用域同名的函数将作为重载函数扩展当前作用域的重载函数
	//如果引入的同名函数形参列表也相同会报错
	//using指示将命名空间成员提升到外层作用域，同名函数形参列表也相同不会产生错误，
	//使用时指定命名空间即可
	//头文件如果在其顶层作用域中有using声明或using指示，则会将名字注入到所有包含了该头文件的文件中
	using std::swap;
	//先将message从各自的Folder记录中移除
	for(auto f : lhs.folders)
		f->remMsg(&lhs);
	for(auto f : rhs.folders)
		f->remMsg(&rhs);
	//交换folder呢消息内容
	//下面会根据函数匹配规则找到合适的版本
	//分别是swap(set &, set&)、swap(string &, string &)
	swap(lhs.folders, rhs.folders);
	swap(lhs.contents, rhs.contents);
	//再将message放回各自Folder中
	for(auto f : lhs.folders)
		f->addMsg(&lhs);
	for(auto f : rhs.folders)
		f->addMsg(&rhs);
}
		

//移动message，函数的意义是用this来代替m
//m保存了它所在的folder set
//所以先将this的folder set指向m的folder set
//然后从每一个folder中移除m
//然后再向每一个folder中添加this
void Message::move_Folders(Message *m) {
	//注意std::move将其转换成右值，它本身并没有任何操作msgs的值
	//真正起作用的是=号，因为是右值，所以实际是调用了set的移动赋值运算符=，
	//是set的移动赋值运算符=移动了msgs指针,如果没有使用std::move是左值，则调用set的拷贝赋值运算符，
	//对这里的理解十分重要
	folders = std::move(m->folders); //使用set的移动赋值运算符
	for(auto f : folders) {          //对于每一个foler
		f->remMsg(m);             //移除旧的message
		f->addMsg(this);          //将this添加到folder中
	}
	//保证销毁m是无害的,
	//经过set的移动赋值运算符调用后,m->folder是有效的，但是包含的内容是不确定的
	//析构函数需要遍历folders,所以我们希望它是空的，这杨销毁m也不会出问题
	m->folders.clear();              
}

//将message this 添加到message m所在的每一个folder
void Message::add_to_Folders(const Message &m) {
	for(auto f : m.folders)
		f->addMsg(this);
}

//将message this从它所在的所有folder中移除
void Message::remove_from_Folders() {
	for(auto f : folders)
		f->remMsg(this);
	//将this所在的folder清空
	folders.clear();
}

//拷贝构造函数
//拷贝m得到一个副本，副本应该和原来的message出现在同一个folder中
Message::Message(const Message &m) : 
	//执行string和set的拷贝构造函数
	contents(m.contents), folders(m.folders)
{
	//将自己添加到每一个folder中
	add_to_Folders(m);
}

//拷贝赋值运算符
//拷贝赋值运算符通常需要执行拷贝构造函数和析构函数的工作
//拷贝右侧对象到左侧对象，并析销毁原来的左侧对象
//当运算符左侧和右侧是同一个对象时也能正确运行
//拷贝赋值运算符应该返回左侧运算对象引用
Message &Message::operator=(const Message &m) {
	//先删除指针，再插入他们来处理自赋值情况
	remove_from_Folders();
	contents = m.contents;
	folders = m.folders;
	add_to_Folders(m);

	return *this;
}

//移动构造函数
//移动构造函数和移动赋值运算符在set插入元素时可能抛出异常，因此不使用noexcept
Message::Message(Message &&m) :
	//使用string的移动构造函数
	contents(std::move(m.contents))
{
	//将m的folder set移动到到this
        //将m从原来的每个folder中移除
	//并将this添加到m原来的每个folder
	//保证移后源m析构是无害的
	move_Folders(&m);
}

//移动赋值运算符
//移动赋值运算符必须首先销毁左侧运算对象
//移动赋值运算符必须保证自赋值是正确的
//移动赋值运算符必须保证移后源析构是无害的
//移动赋值运算符应该返回左侧运算对象引用
Message &Message::operator=(Message &&m) {
	//检查自赋值情况
	if(this != &m) {
		//将this从原来所有的folder中移除
		remove_from_Folders();
		contents = std::move(m.contents);
		//将m的folder set移动到this
		//将m从原来的每个folder中移除
		//并将this添加到m原来的每个folder中
		move_Folders(&m);
	}

	return *this;
}

//析构函数
//将this从所有folder中移除,确保没有任何Folder保存正在销毁的Message
Message::~Message() {
	remove_from_Folders();
}

//将Message添加到给定folder
void Message::save(Folder &f) {
	folders.insert(&f);
	f.addMsg(this);
}

//将Message从给定Folder中移除
void Message::remove(Folder &f) {
	folders.erase(&f);
	f.remMsg(this);
}

void Message::debug_print() {
	cerr << "Message:\n\t" << contents << endl;
	cerr << "Appears in " << folders.size() << " Folders" << endl;
}

//移动folder，函数的意义是用this来替换f
//f保存了它目录下的message set，而每个message右保存了它所在的folder set
//所以要先让this的message set指向f的message set
//然后将从每一个message所在的folder set中移除f,因为message从f移动到了this
//然后再向每一个message所在的folder set中添加this,原因和上面一样
void Folder::move_Messages(Folder *f) {
	msgs = std::move(f->msgs);
	//清空源f的msgs指针set，保证销毁是无害的
	f->msgs.clear();
	//移除源message的folder f,添加this
	for(auto m : msgs) {
		m->remFldr(f);
		m->addFldr(this);
	}
}

//将this添加到f所有message的folder set中
void Folder::add_to_Messages(const Folder &f) {
	for(auto msg : f.msgs)
		msg->addFldr(this);
}

//将this从所有message中移除,我们可以理解当前目录将要销毁了，
//那么应该将目录从它的所有message 的folder set中移除
void Folder::remove_from_Msgs() {
	while(!msgs.empty())
		//注意这个remove是Message::remove
		//这个操作同时会this中的message删除掉
		(*msgs.begin())->remove(*this);
}

//将message添加到this folder
//message所在的folder set也要添加this
void Folder::save(Message &m) {
	msgs.insert(&m);
	m.addFldr(this);
}

//将message从this folder移除
void Folder::remove(Message &m) {
	msgs.erase(&m);
	m.remFldr(this);
}

//拷贝构造函数
Folder::Folder(const Folder &f) : msgs(f.msgs) {
	add_to_Messages(f);
}

//拷贝赋值运算符
Folder &Folder::operator=(const Folder &f) {
	remove_from_Msgs();
	msgs = f.msgs;
	add_to_Messages(f);

	return *this;
}

//移动构造函数
Folder::Folder(Folder &&f) {
	move_Messages(&f);
}

//移动赋值运算符
Folder &Folder::operator=(Folder &&f) {
	if(this != &f) {
		remove_from_Msgs();
		move_Messages(&f);
	}

	return *this;
}

//析构函数
Folder::~Folder() {
	remove_from_Msgs();
}

void Folder::debug_print() {
	cerr << "Folder contains " << msgs.size() << " messages" << endl;
	int ctr = 1;
	for(auto m : msgs) {
		cerr << "Message " << ctr++ << ":\n\t" << m->contents << endl;
	}
}
	
