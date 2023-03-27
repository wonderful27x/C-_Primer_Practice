#ifndef DUMP_TOOL_H
#define DUMP_TOOL_H

#include "dump_base.h"

#include <sstream>

namespace tool
{

// 模板继承模板
// 宇宙全能模板，能dump任何数据
template<typename T>
class UniversalDump : public DumpBase<T>
{
public:
    // 构造函数的继承
    using DumpBase<T>::DumpBase;

protected:
    virtual void GetData(const T &dataSource, DUMP_DATA_LIST &dataList) override
    {
        dataSource.FeedData(dataList);
    }
};

//特例化模板
//可直接用于裸数据的dump, 将char指针和size封装到vector<pair>中
template<>
class UniversalDump<DUMP_DATA_LIST> final : public DumpBase<DUMP_DATA_LIST>
{
public:
    using DumpBase<DUMP_DATA_LIST>::DumpBase;

protected:
    virtual void GetData(const DUMP_DATA_LIST &dataSource, DUMP_DATA_LIST &dataList) override
    {
        dataList = dataSource;
    }
};

// 类模板继承模板类
// 可直接传递数组的引用, 每次dump的size N是一样的
// N 是非类型模板参数
// 不建议直接使用
template<typename T, unsigned N>
class ArrayDumpBaseTemplate : public DumpBase<T>
{
public:
    using DumpBase<T>::DumpBase;

    std::streamsize Write(const char (&arr) [N], int64_t timeNow)
    {
        return DumpBase<T>::Write(arr, timeNow); 
    }

protected:
    /* 我们想隐藏父类的write，但使用using会导致同名函数调用不明确的bug无法通过编译， */
    /* 其实不声明为private也不会导致调用问题，因为根据名字查找首先会找到当前类的Write， */
    /* 再匹配参数，这时如果传递的不是数组引用而是const指针则会报错而不会调用父类的Write */
    /* using DumpBase<T>::Write; */

    virtual void GetData(const T &data, DUMP_DATA_LIST &dataList) override
    {
        dataList.push_back(std::make_pair((const char*)data, N));
    }
};

// 这种方式好像很丑陋
// 类模板继承模板类
// 可直接传递数组的引用, 每次dump的size N是一样的
template<unsigned N>
class ArrayDumpY : public ArrayDumpBaseTemplate<const char *, N>
{
public:
    using ArrayDumpBaseTemplate<const char *, N>::ArrayDumpBaseTemplate;
};

// 使用模板别名, 固定一个参数, 比上面那种方式优雅很多
template <unsigned N> using ArrayDumpZ = ArrayDumpBaseTemplate<const char *, N>;

using CHAR_POINT_TYPE = const char *;
// 类模板继承模板类
// 可直接传递数组的引用, 每次dump的size N是一样的
// 直接使用char*编译无法通过
template<unsigned N>
class ArrayDumpX final : public DumpBase<CHAR_POINT_TYPE>
{
public:
    using DumpBase<CHAR_POINT_TYPE>::DumpBase;

    // 重载父类的write
    std::streamsize Write(const char (&arr) [N], int64_t timeNow)
    {
       return DumpBase<CHAR_POINT_TYPE>::Write(arr, timeNow);
    }

protected:
    /* 我们想隐藏父类的write，但使用using会导致同名函数调用不明确的bug无法通过编译， */
    /* 其实不声明为private也不会导致调用问题，因为根据名字查找首先会找到当前类的Write， */
    /* 再匹配参数，这时如果传递的不是数组引用而是const指针则会报错而不会调用父类的Write */
    /* using DumpBase<T>::Write; */

    virtual void GetData(const CHAR_POINT_TYPE &data, DUMP_DATA_LIST &dataList) override
    {
        dataList.push_back(std::make_pair(data, N));
    }
};

// 类继承模板类
class ArrayDump : public DumpBase<CHAR_POINT_TYPE>
{
public:
    using DumpBase<CHAR_POINT_TYPE>::DumpBase;

    // 重载父类的方法
    // 成员函数模板
    template<unsigned N>
    std::streamsize Write(const char (&arr) [N], int64_t timeNow)
    {
        _size = N;
        return DumpBase<CHAR_POINT_TYPE>::Write(arr, timeNow);
    }

protected:
    std::streamsize _size = 0;
    /* 我们想隐藏父类的write，但使用using会导致同名函数调用不明确的bug无法通过编译， */
    /* 其实不声明为private也不会导致调用问题，因为根据名字查找首先会找到当前类的Write， */
    /* 再匹配参数，这时如果传递的不是数组引用而是const指针则会报错而不会调用父类的Write */
    /* using DumpBase<T>::Write; */

    virtual void GetData(const CHAR_POINT_TYPE &data, DUMP_DATA_LIST &dataList) override
    {
        dataList.push_back(std::make_pair(data, _size));
    }
};

}

#endif
