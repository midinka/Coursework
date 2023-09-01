#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <iostream>
class logger{
public:
    enum class severity{
        trace,
        debug,
        information,
        warning,
        error,
        critical
    };
    virtual ~logger(){

    };
    virtual logger const* log(const std::string& target, severity) const = 0;
};



#endif //LOGGER_LOGGER_H
