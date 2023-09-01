#ifndef ALLOCATORS_MEMORY_H
#define ALLOCATORS_MEMORY_H

#include <iostream>
#include "../logger/logger.h"
class memory{
public:
    virtual void* allocate(size_t target_size) const = 0;

    virtual void deallocate(void* result) const = 0;
    
    virtual ~memory(){

    }
};
#endif //MEMORY_H
