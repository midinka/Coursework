#ifndef LOGGER_LOGGER_CONCRETE_H
#define LOGGER_LOGGER_CONCRETE_H

#include "logger.h"
#include <map>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

class logger_concrete final: public logger{
    friend class logger_builder_concrete;
private:
    std::map<std::string, std::pair<std::ofstream* , logger::severity>> _streams;
    static std::map<std::string, std::pair<std::ofstream* , size_t>> _global_streams;//путь к файлу , файловый указатель, сколько пользователей у этого файла
    static std::string current_datetime_to_string()
    {
        auto time = std::time(nullptr);

        std::ostringstream result_stream;
        result_stream<< std::put_time(std::localtime(&time), "%d.%m.%Y %H:%M:%S");

        return result_stream.str();
    }
public:
    logger_concrete(const logger_concrete& other)=delete;

    logger_concrete(logger_concrete&& other)=delete;

    logger_concrete& operator=(const logger_concrete& other)=delete;

    logger_concrete& operator=(logger_concrete&& other)=delete;
public:
    logger_concrete( std::map<std::string, logger::severity> const& inputmap){
        for(auto &target : inputmap){
            std::ofstream* stream = nullptr;
            auto global_stream=_global_streams.find(target.first);
            if (global_stream==_global_streams.end()){
                if (target.first!="console"){
                    stream=new std::ofstream;
                    stream->open(target.first);
                }
                _global_streams.insert(std::make_pair(target.first, make_pair(stream, 1)));
            }else{
                stream=global_stream->second.first;
                global_stream->second.second++;

            }
            _streams.insert(std::make_pair(target.first, std::make_pair(stream, target.second)));



        }
    }
public:
    ~logger_concrete(){
        for(auto target : _streams){
            auto global_stream=_global_streams.find(target.first);
            if (global_stream->second.second==1){
                if (global_stream->second.first!=nullptr){
                    global_stream->second.first->flush();//сброс на диск
                    global_stream->second.first->close();
                    delete global_stream->second.first;

                }
                _global_streams.erase(global_stream);


            }else{
                global_stream->second.second--;
            }
        }

    }
    logger const* log(const std::string& to_log, logger::severity severety) const{
        for (auto &target : _streams){
            if (target.second.second>severety){
                continue;
            }
            std::map<logger::severity, std::string> level;
            level={
                    {logger::severity::trace, "trace"},
                    {logger::severity::debug, "debug"},
                    {logger::severity::information, "information"},
                    {logger::severity::warning, "warning"},
                    {logger::severity::error, "error"},
                    {logger::severity::critical, "critical"}
            };
            if (target.second.first== nullptr){
                std::cout<<"["<<level[severety]<<"]"<<"["<<current_datetime_to_string()<<"] "<<to_log<<std::endl;
            }else{
                (*target.second.first)<<"["<<level[severety]<<"]"<<"["<<current_datetime_to_string()<<"] "<<to_log<<std::endl;
            }
        }
        return this;

    }



};
std::map<std::string, std::pair<std::ofstream *, size_t> > logger_concrete::_global_streams =std::map<std::string, std::pair<std::ofstream *, size_t> >();

#endif //#ifndef LOGGER_CONCRETE_H
#define LOGGER_CONCRETE_H

#include "logger.h"
#include <map>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

class logger_concrete final: public logger{
    friend class logger_builder_concrete;
private:
    std::map<std::string, std::pair<std::ofstream* , logger::severity>> _streams;
    static std::map<std::string, std::pair<std::ofstream* , size_t>> _global_streams;//путь к файлу , файловый указатель, сколько пользователей у этого файла
    static std::string current_datetime_to_string()
    {
        auto time = std::time(nullptr);

        std::ostringstream result_stream;
        result_stream<< std::put_time(std::localtime(&time), "%d.%m.%Y %H:%M:%S");

        return result_stream.str();
    }
public:
    logger_concrete(const logger_concrete& other)=delete;

    logger_concrete(logger_concrete&& other)=delete;

    logger_concrete& operator=(const logger_concrete& other)=delete;

    logger_concrete& operator=(logger_concrete&& other)=delete;
public:
    logger_concrete( std::map<std::string, logger::severity> const& inputmap){
        for(auto &target : inputmap){
            std::ofstream* stream = nullptr;
            auto global_stream=_global_streams.find(target.first);
            if (global_stream==_global_streams.end()){
                if (target.first!="console"){
                    stream=new std::ofstream;
                    stream->open(target.first);
                }
                _global_streams.insert(std::make_pair(target.first, make_pair(stream, 1)));
            }else{
                stream=global_stream->second.first;
                global_stream->second.second++;

            }
            _streams.insert(std::make_pair(target.first, std::make_pair(stream, target.second)));



        }
    }
public:
    ~logger_concrete(){
        for(auto target : _streams){
            auto global_stream=_global_streams.find(target.first);
            if (global_stream->second.second==1){
                if (global_stream->second.first!=nullptr){
                    global_stream->second.first->flush();//сброс на диск
                    global_stream->second.first->close();
                    delete global_stream->second.first;

                }
                _global_streams.erase(global_stream);


            }else{
                global_stream->second.second--;
            }
        }

    }
    logger const* log(const std::string& to_log, logger::severity severety) const{
        for (auto &target : _streams){
            if (target.second.second>severety){
                continue;
            }
            std::map<logger::severity, std::string> level;
            level={
                    {logger::severity::trace, "trace"},
                    {logger::severity::debug, "debug"},
                    {logger::severity::information, "information"},
                    {logger::severity::warning, "warning"},
                    {logger::severity::error, "error"},
                    {logger::severity::critical, "critical"}
            };
            if (target.second.first== nullptr){
                std::cout<<"["<<level[severety]<<"]"<<"["<<current_datetime_to_string()<<"] "<<to_log<<std::endl;
            }else{
                (*target.second.first)<<"["<<level[severety]<<"]"<<"["<<current_datetime_to_string()<<"] "<<to_log<<std::endl;
            }
        }
        return this;

    }



};
std::map<std::string, std::pair<std::ofstream *, size_t> > logger_concrete::_global_streams =std::map<std::string, std::pair<std::ofstream *, size_t> >();

#endif //LOGGER_LOGGER_CONCRETE_H
