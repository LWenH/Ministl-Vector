#ifndef MINISTL_CONSTRUCT_H
#define MINISTL_CONSTRUCT_H

//This header contains two functions,construct and destroy
//construct response to the construct of the object
//destroy response to release the object

#include <new>
#include "type_traits.h"
#include "util.h"
#include "iterator.h"

namespace ministl
{
    //construct
    template <class T>
    void construct(T *ptr)
    {
        ::new((void *) ptr) T();
    }

    template <class _Tp1,class _Tp2>
    void construct(_Tp1* ptr,const _Tp2 & value)
    {
        ::new((void*) ptr) _Tp1(value);
    }

    template <class T, class ...Args>
    void construct(T* ptr, Args &&... args)
    {
        ::new((void*) ptr) T(ministl::forward<T>(args)...);
    }

    //destroy------>析构对象
    template <class T>
    void destroy_one(T*, std::true_type){}

    template <class T>
    void destroy_one(T* ptr, std::false_type)
    {
        if(ptr != nullptr)
        {
            ptr->~T();
        }
    }

    template <class ForwardIter>
    void destroy_cat(ForwardIter,ForwardIter,std::true_type){}

    template <class ForwardIter>
    void destroy_cat(ForwardIter first,ForwardIter last,std::false_type)
    {
        for(;first != last;++first)
        {
            destroy(&*first);
        }
    }

    template <class T>
    void destroy(T* ptr)
    {
        destroy_one(ptr,std::is_trivially_destructible<T>{});
    }

    template <class ForwardIter>
    void destroy(ForwardIter first,ForwardIter last)
    {
        destroy_cat(first,last,std::is_trivially_destructible<
                typename iterator_traits<ForwardIter>::value_type>{});
    }
}







#endif //MINISTL_CONSTRUCT_H
