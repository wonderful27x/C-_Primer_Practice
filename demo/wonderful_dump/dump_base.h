#ifndef DUMP_BASE_H
#define DUMP_BASE_H

#include <fstream>
#include <string>
#include <vector>
#include <exception>

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
            DumpMode dumpMode = DumpMode::MOPT_ONCE,
            std::streamsize maxDumpSize = INT64_MAX,
            int64_t maxDumpDurationMs = INT64_MAX,
            uint32_t startOffset = 0
            ) :
        _dump_dir(dumpDir),
        _dump_f_name(dumpFilename),
        _dump_mode(dumpMode),
        _start_offset(startOffset),
        _max_dump_size(maxDumpSize),
        _dump_size(0),
        _max_dump_duration(maxDumpDurationMs),
        _dump_duration(0),
        _time_bg(INT64_MAX),
        _time_tmp(-1),
        _finish(false)
    {}

    virtual ~DumpBase()
    {
        if(_file.is_open())
        {
            _file.close();
        }
    }

    int64_t GetDumpDuration()
    {
        return _dump_duration;
    }

    std::streamsize GetDumpSize()
    {
        return _dump_size;
    }

    bool Finish()
    {
        return _finish;
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

        // jump time offset
        if(timeNow > 0)
        {
            if(_start_offset != 0 && _time_tmp == -1)
            {
                _time_tmp = timeNow;
            }
            if(timeNow - _time_tmp < _start_offset)
            {
                return 0;
            }
            else
            {
                _start_offset = 0;
            }
        }

        if(_time_bg == INT64_MAX && !_file.is_open())
        {
            std::string file_path = GetFilePath(dataSource);
            LogPrint("Open file for data dumping, file path: " + file_path);
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
            if(_dump_duration > _max_dump_duration || _dump_size >= _max_dump_size)
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
                std::string dump_msg = "Dump finish, dump size: " + std::to_string(_dump_size) + " bytes, dump duration: " + std::to_string(_dump_duration) + " ms";
                LogPrint(dump_msg);
            }
            else
            {
                DUMP_DATA_LIST data_list;
                GetData(dataSource, data_list);
                for(auto dp : data_list)
                {
                    write_size += WriteData(dp.first, dp.second);
                }
                _dump_size += write_size;
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
        std::string f_name = _dump_f_name.substr(0, position) + "_" + std::to_string((long long)this) + "_" + _dump_f_name.substr(position, _dump_f_name.size() - position);
        return _dump_dir + f_name;
    }

    virtual void GetData(const T &dataSource, DUMP_DATA_LIST &dataList) = 0;

    virtual void LogPrint(const std::string &message)
    {
        std::cout << message << std::endl;
    }

private:
    std::fstream _file;
    DumpMode _dump_mode;

    uint32_t _start_offset;

    // note: if (_max_dump_size || _max_dump_duration) stop
    const std::streamsize _max_dump_size;
    std::streamsize _dump_size;
    const int64_t _max_dump_duration;
    int64_t _dump_duration;

    int64_t _time_bg;
    int64_t _time_tmp;

    bool _finish;

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
