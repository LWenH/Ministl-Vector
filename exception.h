#ifndef MINISTL_EXCEPTION_H
#define MINISTL_EXCEPTION_H

//This header define a series of exception

#include <stdexcept>
#include <cassert>


namespace ministl
{
#define MINISTL_DEBUG(expr)\
    assert(expr)

#define THROW_LENGTH_ERROR_IF(expr,what) \
    if((expr)) throw std::length_error(what)

#define THROW_OUT_OF_RANGE_IF(expr,what) \
    if((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr,what) \
    if((expr)) throw std::runtime_error(what)
}

#endif //MINISTL_EXCEPTION_H
