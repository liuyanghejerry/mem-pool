#ifndef COMMON_H
#define COMMON_H
#pragma once

#include <boost/interprocess/shared_memory_object.hpp>

namespace common {

enum OPEN_MODE {
    OPEN_ONLY = 0x1,
    CREATE_ONLY = 0x2,
    OPEN_OR_CREATE = 0x3
};
enum ACCESS_MODE {
    READ = 0x1,
    READ_WRITE = 0x3,
    COPY_ON_WRITE
};

static inline boost::interprocess::mode_t toAccessMode(ACCESS_MODE m)
{
    using namespace boost::interprocess;

    boost::interprocess::mode_t r = read_only;
    switch(m) {
    case READ:
        r = read_only;
        break;
    case READ_WRITE:
        r = read_write;
        break;
    case COPY_ON_WRITE:
        r = copy_on_write;
        break;
    }
    return r;
}

}

#endif
