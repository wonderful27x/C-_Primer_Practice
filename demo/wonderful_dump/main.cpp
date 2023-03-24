#include <iostream>
#include <string>
#include <sys/time.h>  // gettimeofday
#include <sys/timeb.h> // ftime
#include <time.h>      // time clock_gettime
#include <unistd.h>
#include <vector>

#include "dump_tool.h"

struct DumpData
{
    DumpData(const char *data, std::streamsize size) : _data(data), _size(size) {}

    const char *_data = nullptr;
    std::streamsize _size = 0;

    void FeedData(std::vector<std::pair<const char *, std::streamsize>> &dataList) const
    {
        dataList.push_back(std::make_pair(_data, _size));
    }

};

long long get_system_time_millisecond()
{
    struct timeb timestamp = {};

    if (0 == ftime(&timestamp))
        return timestamp.time * 1000 + timestamp.millitm;
    else
        return 0;
}

// 没100ms循环一次, 后面传递的都是运行15s
void loop(long long duration, int (*p) (long long time))
{
    long long tbg = get_system_time_millisecond();
    long long start = tbg;
    long long ted;
    int result = 0;
    do{
         ted = get_system_time_millisecond();
        if(ted - tbg >= 100)
        {
            result = p(ted);
            tbg = get_system_time_millisecond();
        }
    }while((ted - start < duration) && result >= 0);
}

const char data[9] = {'a','b','c','d','e','f','g','h','i'};
std::vector<std::pair<const char *, std::streamsize>> dataList;
DumpData dumpData(data, 9);

// 最大写入9个字节
tool::UniversalDump<DumpData> universal_dump("./",
        "universal-dumpdata.txt",
        tool::DumpMode::MOPT_ONCE,
        9);
int test_UniversalDump(long long time)
{
    return universal_dump.Write(dumpData, time);
}

// 偏移1秒，最多写入5秒大概是 5*10*9 = 450 bytes
tool::UniversalDump<tool::DUMP_DATA_LIST> universal_dump_list("./",
        "universal-dumpdatalist.txt",
        tool::DumpMode::MOPT_ONCE,
        INT64_MAX,
        5000,
        1000);
int test_ListDataDump(long long time)
{
    return universal_dump_list.Write(dataList, time);
}

// 循环写，最大写入27字节
tool::ArrayDumpBaseTemplate<const char *, 9> array_base_dump("./",
        "universal-dump-arraybase.txt",
        tool::DumpMode::MOPT_ONCE,
        27,
        INT64_MAX);
int test_ArrayBaseDump(long long time)
{
    // for test
    /* const char *d = "abc"; */
    return array_base_dump.Write(data, time);
}

// 循环写，最大写入27字节
tool::ArrayDumpY<9> array_dumpy("./",
        "universal-dump-arrayy.txt",
        tool::DumpMode::MOPT_ONCE,
        27,
        INT64_MAX);
int test_ArrayDumpY(long long time)
{
    return array_dumpy.Write(data, time);
}

// 循环写，最大写入27字节
tool::ArrayDumpZ<9> array_dumpz("./",
        "universal-dump-arrayz.txt",
        tool::DumpMode::MOPT_ONCE,
        27,
        INT64_MAX);
int test_ArrayDumpZ(long long time)
{
    return array_dumpz.Write(data, time);
}

// 循环写，最大写入27字节
tool::ArrayDumpX<9> array_dumpx("./",
        "universal-dump-arrayx.txt",
        tool::DumpMode::MOPT_ONCE,
        27,
        INT64_MAX);
int test_ArrayDumpX(long long time)
{
    return array_dumpx.Write(data, time);
}

// 循环写，最大写入27字节
tool::ArrayDump array_dump("./",
        "universal-dump-array.txt",
        tool::DumpMode::MOPT_LOOP,
        90,
        INT64_MAX);
int test_ArrayDump(long long time)
{
    return array_dump.Write(data, time);
}

int main()
{
    dataList.push_back(std::make_pair(data, 9));

    loop(15000, test_UniversalDump);
    loop(15000, test_ListDataDump);
    loop(15000, test_ArrayBaseDump);
    loop(15000, test_ArrayDumpY);
    loop(15000, test_ArrayDumpZ);
    loop(15000, test_ArrayDumpX);
    loop(5000, test_ArrayDump);

    return 0;
}
