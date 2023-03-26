#ifndef DUMP_BASE_H
#define DUMP_BASE_H

#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include <sstream>

namespace tool
{

enum class DumpMode
{
    AUTO_ONCE,      //if dir no exist create it, clear first, when reach dumpDurationMs stop
    AUTO_LOOP,      //if dir no exist create it, clear first, when reach dumpDurationMs overwrite again
    AUTO_APPEND,    //if dir no exist create it, then append data to the file 
    MOPT_ONCE,      //if dir no exist no write, otherwise, clear first, when reach dumpDurationMs stop 
    MOPT_LOOP,      //if dir no exist no write, otherwise, clear first, when reach dumpDurationMs overwrite again
    MOPT_APPEND     //if dir no exist no write, otherwise, then append data to the file 
};

using DUMP_DATA_LIST = std::vector<std::pair<const char *, std::streamsize>>;

template<typename T>
class DumpBase
{
public:
    DumpBase(const std::string &dumpDir,
            const std::string &dumpFilename,
            const DumpMode dumpMode = DumpMode::MOPT_ONCE,
            const std::streamsize maxDumpSize = INT64_MAX,
            const int64_t maxDumpDurationMs = INT64_MAX,
            const uint32_t startOffset = 0,
            const uint32_t intervalNum = 0
            ) :
        _dump_dir(dumpDir),
        _dump_f_name(dumpFilename),
        _dump_mode(dumpMode),
        _start_offset(startOffset),
        _start_offset_ctr(startOffset),
        _max_dump_size(maxDumpSize),
        _max_dump_duration(maxDumpDurationMs),
        _interval_num(intervalNum)
    {}

    virtual ~DumpBase()
    {
        if(_file.is_open())
        {
            _file.close();
        }
    }

    inline
    int64_t GetDumpDuration()
    {
        return _dump_duration;
    }

    inline
    std::streamsize GetDumpSize()
    {
        return _dump_size;
    }

    inline
    bool Finish()
    {
        return _finish;
    }

    inline
    int64_t DumpCount()
    {
        return _dump_count;
    }

    inline
    int64_t WriteCount()
    {
        return _write_count;
    }

    // note: if timeNow < 0, _max_dump_duration and _start_offset will not take effert
    std::streamsize Write(const T &dataSource, int64_t timeNow)
    {
        // TODO
        if(_dump_mode == DumpMode::AUTO_ONCE ||
                _dump_mode == DumpMode::AUTO_LOOP ||
                _dump_mode == DumpMode::AUTO_APPEND)
        {
            throw std::string("No support this DumpMode: ") + std::to_string((int)_dump_mode) + " for dir create no implemented!!!";
        }

        _dump_count++;

        // jump time offset
        if(timeNow > 0)
        {
            if(_start_offset_ctr != 0 && _time_tmp == -1)
            {
                _time_tmp = timeNow;
            }
            if(timeNow - _time_tmp < _start_offset_ctr)
            {
                return 0;
            }
            else
            {
                _start_offset_ctr = 0;
            }
        }

        if(_time_bg == INT64_MAX && !_file.is_open())
        {
            std::string file_path = GetFilePathPrivate(dataSource);
            std::stringstream sstr;
            sstr << "Open file for data dumping, dump mode: "
                << (int)_dump_mode
                << " , time offset: "
                << _start_offset
                << " ms, max dump size: "
                << _max_dump_size
                << " , max dump duration: "
                << _max_dump_duration
                << " , interval number: "
                << _interval_num
                << " , file path: "
                << GetFilePathPrivate(dataSource);
            LogPrint(sstr.str());

            if(_dump_mode == DumpMode::AUTO_ONCE ||
                    _dump_mode == DumpMode::AUTO_LOOP ||
                    _dump_mode == DumpMode::MOPT_ONCE ||
                    _dump_mode == DumpMode::MOPT_LOOP)
            {
                _file.open(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
                _dump_size = 0;
            }
            else if(_dump_mode == DumpMode::AUTO_APPEND || _dump_mode == DumpMode::MOPT_APPEND)
            {
                _file.open(file_path, std::ios::out | std::ios::app | std::ios::binary);
            }
            if(_file.is_open())
            {
                _time_bg = timeNow;
            }
            else
            {
                LogPrint("Faild to open file for data dumping, file path: " + file_path);
            }
        }

        std::streamsize write_size = 0;
        if(_file.is_open())
        {
            _dump_duration = timeNow - _time_bg;
            if(_dump_duration <= _max_dump_duration && _dump_size < _max_dump_size)
            {
                if(!Jump(_dump_count, _write_count, _interval_num, dataSource, timeNow))
                {
                    DUMP_DATA_LIST data_list;
                    GetData(dataSource, data_list);
                    for(auto dp : data_list)
                    {
                        write_size += WriteData(dp.first, dp.second);
                    }
                    _dump_size += write_size;
                    _write_count++;
                }
            }

            if(_dump_duration >= _max_dump_duration || _dump_size >= _max_dump_size)
            {
                _file.close();
                if(_dump_mode == DumpMode::AUTO_LOOP ||
                        _dump_mode == DumpMode::MOPT_LOOP)
                {
                    _time_bg = INT64_MAX;
                }
                else
                {
                    _finish = true;
                    write_size = -99;
                }
                std::stringstream sstr;
                sstr << "Dump finish, dump size: "
                    << _dump_size
                    << " bytes, dump duration: "
                    << _dump_duration
                    << " ms, dump count: "
                    << _dump_count
                    << ", write count: "
                    << _write_count;
                LogPrint(sstr.str());
            }
        }

        return write_size;
    }

protected:
    const std::string _dump_dir;
    const std::string _dump_f_name;

    virtual std::string GetFilePath(const T &)
    {
        std::string::size_type  position = _dump_f_name.rfind(".");
        std::stringstream sstr;
        sstr << _dump_dir
            << _dump_f_name.substr(0, position)
            << "_"
            << this
            << "_"
            << _dump_f_name.substr(position, _dump_f_name.size() - position);

        return sstr.str();
    }

    virtual void GetData(const T &dataSource, DUMP_DATA_LIST &dataList) = 0;

    /* 1 2 : 2%2  3%2j 4%2  5%2j 6%2  7%2j */
    /* 2 3 : 3%3  4%3j 5%3j 6%3  7%3j 8%3j 9%3   10%3j 11%3j 12%3 */
    /* 3 4 : 4%4  5%4j 6%4j 7%4j 8%4  9%4j 10%4j 11%4j 12%4 13%4j 14%4j 15%4j 16%4 */
    virtual bool Jump(uint64_t currentCount, uint64_t writeCount, uint32_t initIntervalNum, const T &t, int64_t timeNow)
    {
        if(initIntervalNum != 0 && (currentCount + initIntervalNum) % (initIntervalNum + 1) != 0)
        {
            return true;
        }

        return false;
    }

    virtual void LogPrint(const std::string &message)
    {
        std::cout << message << std::endl;
    }

private:
    std::fstream _file;
    const DumpMode _dump_mode;

    const uint32_t _start_offset;
    uint32_t _start_offset_ctr;

    // note: if (_max_dump_size || _max_dump_duration) stop
    const std::streamsize _max_dump_size;
    std::streamsize _dump_size = 0;
    const int64_t _max_dump_duration;
    int64_t _dump_duration = 0;

    int64_t _time_bg = INT64_MAX;
    int64_t _time_tmp = -1;

    bool _finish = false;

    const uint32_t _interval_num;
    int64_t _dump_count = 0;
    int64_t _write_count = 0;

    inline
    std::string GetFilePathPrivate(const T &t)
    {
        if(_dump_mode == DumpMode::AUTO_APPEND ||
                _dump_mode == DumpMode::MOPT_APPEND)
        {
            return _dump_dir + _dump_f_name;
        }
        return GetFilePath(t);
    }

    inline
    std::streamsize WriteData(const char *data, std::streamsize size)
    {
        if(data == nullptr)
        {
            return 0;
        }
        _file.write(data, size);
        
        return size;
    }

    // TODO
    void CreateDir(const std::string &path)
    {
    }

};

}

#endif
