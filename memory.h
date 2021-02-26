#ifndef MINISTL_MEMORY_H
#define MINISTL_MEMORY_H

// This header is responsible for higher level memory management
// Containing of some basic functions, space allocators, uninitialized memory control and a template class auto_ptr's implementation


#include <cstddef>
#include <cstdlib>
#include <climits>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace ministl
{
    //获取对象的地址
    template <class Tp>
    constexpr Tp* address_of(Tp &value) noexcept
    {
        return &value;
    }

    //获取与释放temp buffer
    template <class T>
    pair<T,ptrdiff_t> get_buffer_helper(ptrdiff_t len,T*)
    {
        if(len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);
        while(len > 0)
        {
            T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
            if(tmp)
                return pair<T,ptrdiff_t >(tmp,len);
            len /= 2;   //申请失败大小减半
        }
        return pair<T,ptrdiff_t>(nullptr,0);
    }

    template <class T>
    pair<T,ptrdiff_t> get_temporary_buffer(ptrdiff_t len)
    {
        return get_buffer_helper(len, static_cast<T*>(0));
    }

    template <class T>
    void release_temporary_buffer(T* ptr)
    {
        free(ptr);
    }

    // --------------------------------------------------------------------------------------
    // 类模板 : temporary_buffer
    // 进行临时缓冲区的申请与释放
    template <class ForwardIterator, class T>
    class temporary_buffer
    {
    private:
        ptrdiff_t original_len;  // 缓冲区申请的大小
        ptrdiff_t len;           // 缓冲区实际的大小
        T*        buffer;        // 指向缓冲区的指针

    public:
        // 构造、析构函数
        temporary_buffer(ForwardIterator first, ForwardIterator last);

        ~temporary_buffer()
        {
            ministl::destroy(buffer, buffer + len);
            free(buffer);
        }

    public:

        ptrdiff_t size()           const noexcept { return len; }
        ptrdiff_t requested_size() const noexcept { return original_len; }
        T*        begin()                noexcept { return buffer; }
        T*        end()                  noexcept { return buffer + len; }

    private:
        void allocate_buffer();
        void initialize_buffer(const T&, std::true_type) {}
        void initialize_buffer(const T& value, std::false_type)
        { ministl::uninitialized_fill_n(buffer, len, value); }

    private:
        //Allocated buffer forbid copy and assign
        temporary_buffer(const temporary_buffer&);
        void operator=(const temporary_buffer&);
    };

    //constructor
    template <class ForwardIter,class T>
    temporary_buffer<ForwardIter,T>::temporary_buffer(ForwardIter first, ForwardIter last) {
        try {
            len = ministl::distance(first,last);
            allocate_buffer();
            if(len > 0)
                initialize_buffer(*first,std::is_trivially_default_constructible<T>());
        }
        catch (...)
        {
            free(buffer);
            buffer = nullptr;
            len = 0;
        }
    }

    template <class ForwardIter,class T>
    void temporary_buffer<ForwardIter,T>::allocate_buffer() {
        original_len = len;
        if(len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);
        while (len > 0)
        {
            buffer = static_cast<T*>(malloc(len * sizeof(T)));
            if (buffer)
                break;
            len /= 2;  // 申请失败时减少申请空间大小
        }
    }

    // --------------------------------------------------------------------------------------
    // 模板类: auto_ptr
    // 一个具有严格对象所有权的小型智能指针
    template <class T>
    class auto_ptr
    {
    public:
        typedef T       elem_type;

    private:
        //真实指针
        T *m_ptr;
    public:
        explicit auto_ptr(T *p = nullptr) : m_ptr(p) {}
        auto_ptr(auto_ptr &rhs) : m_ptr(rhs.release()){}
        template <class U>
        explicit auto_ptr(auto_ptr<U>& other) : m_ptr(other.release()){}

        auto_ptr& operator= (const auto_ptr & other)
        {
            if(this != &other)
            {
                delete m_ptr;
                m_ptr = other.release();
            }
            return *this;
        }

        template <class U>
        auto_ptr& operator= (auto_ptr<U>& other)
        {
            if(this->get() != other->get())
            {
                delete m_ptr;
                m_ptr = other.release();
            }
            return *this;
        }

        ~auto_ptr(){ delete m_ptr;}

    public:
        T &operator*() const { return *m_ptr;}
        T *operator->() const { return  m_ptr;}

        T* get()
        {
            return m_ptr;
        }

        T* release()
        {
            T* tmp = m_ptr;
            m_ptr = nullptr;
            return tmp;
        }

        void reset(T* p = nullptr)
        {
            if(m_ptr != p)
            {
                delete m_ptr;
                m_ptr = p;
            }
        }
    };

}   //namespace ministl



#endif //MINISTL_MEMORY_H
