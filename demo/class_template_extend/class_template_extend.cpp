#include <iostream>
#include <string>

// 老板
// 抽象基类模板
template<typename T>
class Boss
{
public:
    virtual ~Boss(){}
    // 将工作交给下面的工人做
    void Work(const T &work)
    {
        std::cout << "do work: " << std::endl;
        DoWork(work);
    }

protected:
    // 由子类实现具体的工作
    virtual void DoWork(const T &work) = 0;
};

// 工人
// 最直接的方式，子类继承模板类(类继承类)
class Worker : public Boss<std::string>
{
protected:
    // 工人接到任务开始工作
    virtual void DoWork(const std::string &work) override
    {
        std::cout << "Worker doing work: " << work << std::endl;
    }
};

// 类模板继承类模板(模板继承模板), 这种方式可以理解为相对上面的实现又多了一层封装
// 还有另外两种方式：https://blog.csdn.net/pengwentao/article/details/129028577u
//      1. 类模板继承模板类(模板继承类)
//      2. 类模板继承普通类(模板继承普通类)
//      注意: 类不能继承模板
template<typename T>
class Leader : public Boss<T>
{
protected:
    // 工人接到任务开始工作
    virtual void DoWork(const T &work) override
    {
        std::cout << "Leader doing work: " << work.c_str() << std::endl;
    }
};

// 特例化模板
template<>
class Leader<int> : public Boss<int>
{
protected:
    // 工人接到任务开始工作
    virtual void DoWork(const int &number) override
    {
        std::cout << "Leader<int> doing work: how many workers -> " << number << std::endl;
    }
};

int main()
{
    Boss<std::string> *bossWorker = new Worker;
    bossWorker->Work("老板亲属工人，生产一辆速腾手动飞跃版小汽车");
    delete bossWorker;
    Worker work;
    work.Work("普通工人，生产五辆速腾手动飞跃版小汽车");

    Boss<std::string> *bossLeaderStr = new Leader<std::string>;
    bossLeaderStr->Work("老板亲属领导发红包, 88888888");
    delete bossLeaderStr;
    Leader<std::string> leader;
    leader.Work("普通领导发红包，666666");

    Boss<int> *bossLeaderInt = new Leader<int>;
    bossLeaderInt->Work(10);
    delete bossLeaderInt;
    Leader<int> leaderInt;
    leaderInt.Work(100);

    return 0;
}
