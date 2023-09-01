#ifndef LOGGER_LOGGER_BUILDER_CONCRETE_H
#define LOGGER_LOGGER_BUILDER_CONCRETE_H
#include<map>
#include "logger_builder.h"

class logger_builder_concrete final: public logger_builder{
private:
    std::map<std::string, logger::severity> _construct_info;
public:
    logger_builder* add_stream(std::string const & path, logger::severity severety){
        _construct_info[path]=severety;
        return this;
    }
    logger* construct() const override {
        return new logger_concrete(_construct_info);

    }
    logger_builder* add_in_file(const std::string& way) override {
        std::string file_way, type;
        std::map <std::string, logger::severity> maps;
        maps={
                {"trace", logger::severity::trace},
                {"debug", logger::severity::debug},
                {"information", logger::severity::information},
                {"warning", logger::severity::warning},
                {"error", logger::severity::error},
                {"critical", logger::severity::critical}
        };
        std::ifstream file(way);//проверка
        if (!file.is_open()){
            throw std::logic_error("Could not open file");
        }
        while(file>>file_way>>type){
            add_stream(file_way, maps[type]);
        }
        file.close();
        return this;

    }
};

#endif //LOGGER_LOGGER_BUILDER_CONCRETE_H
