#ifndef MINISTL_ALLOCATOR_H
#define MINISTL_ALLOCATOR_H

//This header contains a template class allocator
//Allocator in order to manage the allocation,releases of the memory
//Also the object construction and destruction


#include "util.h"
#include "construct.h"

namespace ministl
{
    template <class T>
    class allocator
    {
    public:
        typedef T                      value_type;
        typedef T*                     pointer;
        typedef const T*               const_pointer;
        typedef T&                     reference;
        typedef const T&               const_reference;
        typedef size_t                 size_type;
        typedef ptrdiff_t              difference_type;

    public:
        //申请空间
        static T* allocate();
        static T* allocate(size_type n);

        static void deallocate(T* ptr);
        static void deallocate(T* ptr,size_type n);

        static void construct(T* ptr);
        static void construct(T* ptr,const T& value);
        static void construct(T* ptr,T &&value);

        template <class ...Args>
        static void construct(T* ptr,Args&& ...args);

        static void destroy(T* ptr);
        static void destroy(T* first,T* last);
    };

    template<class T>
    T *allocator<T>::allocate() {
        return static_cast<T*> (::operator new(sizeof(T)));
    }

    template<class T>
    T *allocator<T>::allocate(size_type n) {
        if(n == 0)
            return nullptr;
        return static_cast<T*>(::operator new (n * sizeof(T)));
    }

    template<class T>
    void allocator<T>::deallocate(T *ptr) {
        if(ptr == nullptr)
            return;
        ::operator delete(ptr);
    }

    template<class T>
    void allocator<T>::deallocate(T *ptr, allocator::size_type) {
        if(ptr == nullptr)
            return;
        ::operator delete(ptr);
    }

    template<class T>
    void allocator<T>::construct(T *ptr) {
        ministl::construct(ptr);
    }

    template<class T>
    void allocator<T>::construct(T *ptr, const T &value) {
        ministl::construct(ptr,value);
    }

    template<class T>
    void allocator<T>::construct(T *ptr, T &&value) {
        ministl::construct(ptr,ministl::move(value));
    }

    template<class T>
    template<class... Args>
    void allocator<T>::construct(T *ptr, Args &&... args) {
        ministl::construct(ptr,ministl::forward<Args>(args)...);
    }

    template<class T>
    void allocator<T>::destroy(T *ptr) {
        ministl::destroy(ptr);
    }

    template<class T>
    void allocator<T>::destroy(T *first, T *last) {
        ministl::destroy(first,last);
    }

}

#endif //MINISTL_ALLOCATOR_H
