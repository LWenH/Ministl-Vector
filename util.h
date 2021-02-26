#ifndef MINISTL_UTIL_H
#define MINISTL_UTIL_H

//This header define some general utility tools,like swap,pair and move etc

#include <cstddef>
#include <type_traits>

namespace ministl
{
    //move
    template <class T>
    inline typename std::remove_reference<T>::type&& move(T&& arg) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }

    //forward
    template <class T>
    inline T&& forward(typename std::remove_reference<T>::type & arg) noexcept
    {
        //此处传入左值,template 推导将引用折叠成为lvalue reference
        return static_cast<T&&>(arg);
    }

    template <class T>
    inline T&& forward(typename std::remove_reference<T>::type && arg) noexcept
    {
        //右值转发,lvalue 静态检查
        static_assert(!std::is_lvalue_reference<T>(arg),"bad forward");
        return static_cast<T&&>(arg);
    }

    //swap
    template <class T>
    void swap(T& lhs,T& rhs)
    {
        auto tmp(ministl::move(rhs));
        rhs = ministl::move(lhs);
        lhs = ministl::move(tmp);
    }

    //在范围 [first1, last1) 和始于first2的另一范围间交换元素。
    template< class ForwardIt1, class ForwardIt2 >
    ForwardIt2 swap_ranges( ForwardIt1 first1, ForwardIt1 last1,ForwardIt2 first2 )
    {
        //todo fix me 可能出错
        while(first1 != last1)
            ministl::swap(*first1++,*first2++);
        return first2;
    }

    //交换a与b数组,此处要传数组的引用
    template <class Tp, size_t N>
    void swap(Tp (&a)[N],Tp (&b)[N]) noexcept
    {
        ministl::swap_ranges(a,a+N,b);
    }

    //---------------------------------------pair---------------------------------------
    template<class T1,class T2>
    struct pair
    {
        T1  first;
        T2  second;

        //default constructible, use initialize list to construct directly
        template<class _Tp1=T1,class _Tp2=T2,typename = typename std::enable_if<
            std::is_default_constructible<_Tp1>::value &&
            std::is_default_constructible<_Tp2>::value,void >::type>
        constexpr pair() : first(),second()
        {
        }

        //implicit constructible for this type
        //判断是否接受隐式转换（first 和 second 都是非const）,无论如何两个类型都可以接受拷贝构造
        template <class _Tp1=T1,class _Tp2=T2,typename std::enable_if<
            std::is_copy_constructible<_Tp1>::value&&
            std::is_copy_constructible<_Tp2>::value&&
            std::is_convertible<const _Tp1&,T1>::value&&
            std::is_convertible<const _Tp2&,T2>::value,int>::type = 0>
        constexpr pair(const T1& a, const T2& b) : first(a),second(b)
        {
        }

        //explicit constructible for this type
        template <class _Tp1=T1,class _Tp2=T2,typename std::enable_if<
            std::is_copy_constructible<_Tp1>::value&&
            std::is_copy_constructible<_Tp2>::value&&
            (!std::is_convertible<const _Tp1&,T1>::value||
             !std::is_convertible<const _Tp2&,T2>::value),int>::type = 0>
        explicit constexpr pair(const T1& a, const T2& b) : first(a),second(b)
        {
        }

        pair(const pair& lhs) = default;
        pair(pair && rhs) noexcept = default ;

        //implicit constructible from other type
        //用其它类型构造时要确认other type的右值可以绑定到T1和T2的左值来进行移动构造
        template <class _Other1,class _Other2, typename std::enable_if<
            std::is_constructible<T1,_Other1>::value &&
            std::is_constructible<T2,_Other2>::value &&
            std::is_convertible<_Other1&&,T1>::value &&
            std::is_convertible<_Other2&&,T2>::value,int>::type = 0>
        constexpr pair(T1&& a, T2&& b) : first(ministl::forward<T1>(a)),second(ministl::forward<T2>(b))
        {
        }

        //explicit constructible from other type
        template <class _Other1,class _Other2, typename std::enable_if<
            std::is_constructible<T1,_Other1>::value &&
            std::is_constructible<T2,_Other2>::value &&
            (!std::is_convertible<_Other1,T1>::value ||
             !std::is_convertible<_Other2,T2>::value),int>::type = 0>
        explicit constexpr pair(T1&& a, T2&& b) : first(ministl::forward<T1>(a)),second(ministl::forward<T2>(b))
        {
        }

        //implicit constructible from other pair
        //other.first和second都是常量，要绑定到this->first和second
        template <class _Other1,class _Other2, typename std::enable_if<
            std::is_constructible<T1,_Other1>::value &&
            std::is_constructible<T2,_Other2>::value &&
            std::is_convertible<const _Other1&,T1>::value &&
            std::is_convertible<const _Other2&,T2>::value,int>::type = 0>
        constexpr pair(pair<_Other1,_Other2>&& other):first(ministl::forward<_Other1>(other.first)),
        second(ministl::forward<_Other2>(other.second))
        {
        }

        //explicit constructible from other pair
        template <class _Other1,class _Other2,typename std::enable_if<
                std::is_constructible<T1,_Other1>::value &&
                std::is_constructible<T2,_Other2>::value &&
                (!std::is_convertible<const _Other1&,T1>::value ||
                 !std::is_convertible<const _Other2&,T2>::value),int>::type = 0>
        explicit constexpr pair(pair<_Other1,_Other2>&& other):first(ministl::forward<_Other1>(other.first)),
        second(ministl::forward<_Other2>(other.second))
        {
        }

        //implicit constructible from other pair
        //这里的话就是拷贝构造,可以类型转换_Other到T即可
        template <class _Other1,class _Other2, typename std::enable_if<
                std::is_constructible<T1,_Other1>::value &&
                std::is_constructible<T2,_Other2>::value &&
                std::is_convertible<_Other1,T1>::value &&
                std::is_convertible<_Other2,T2>::value,int>::type = 0>
        constexpr pair(const pair<_Other1,_Other2>& other):first(other.first),second(other.second)
        {
        }

        //explicit constructible from other pair
        template <class _Other1,class _Other2,typename std::enable_if<
                std::is_constructible<T1,_Other1>::value &&
                std::is_constructible<T2,_Other2>::value &&
                (!std::is_convertible<_Other1,T1>::value ||
                 !std::is_convertible<_Other2,T2>::value),int>::type = 0>
        explicit constexpr pair(const pair<_Other1,_Other2>& other):first(other.first),second(other.second)
        {
        }

        //拷贝赋值
        pair& operator=(const pair& other)
        {
            if(this != &other)
            {
                first = other.first;
                second = other.second;
            }
            return *this;
        }

        //移动赋值
        pair& operator=(pair &&other) noexcept
        {
            if(this != other)
            {
                first = ministl::move(other.first);
                second = ministl::move(other.second);
            }
            return *this;
        }

        //assign from other type
        template <class _Other1,class _Other2>
        pair& operator=(const pair<_Other1,_Other2>& other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        //move assign from other type
        template <class _Other1,class _Other2>
        pair& operator=(pair<_Other1,_Other2> && other)
        {
            first = ministl::forward<_Other1>(other.first);
            second = ministl::forward<_Other2>(other.second);
            return *this;
        }

        ~pair() = default;

        void swap(pair & other)
        {
            if(this != &other)
            {
                ministl::swap(this->first,other.first);
                ministl::swap(this->second,other.second);
            }
        }
    };

    template <class _Tp1,class _Tp2>
    bool operator == (const pair<_Tp1,_Tp2>& lhs,const pair<_Tp1,_Tp2>& rhs)
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template <class _Tp1,class _Tp2>
    bool operator < (const pair<_Tp1,_Tp2>& lhs,const pair<_Tp1,_Tp2>& rhs)
    {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }

    template <class _Tp1,class _Tp2>
    bool operator != (const pair<_Tp1,_Tp2>& lhs,const pair<_Tp1,_Tp2>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class _Tp1,class _Tp2>
    bool operator > (const pair<_Tp1,_Tp2>& lhs,const pair<_Tp1,_Tp2>& rhs)
    {
        return rhs < lhs;
    }

    template <class _Tp1,class _Tp2>
    bool operator >= (const pair<_Tp1,_Tp2>& lhs,const pair<_Tp1,_Tp2>& rhs)
    {
        return !(lhs < rhs);
    }

    template <class _Tp1,class _Tp2>
    bool operator <= (const pair<_Tp1,_Tp2>& lhs,const pair<_Tp1,_Tp2>& rhs)
    {
        return !(rhs < lhs);
    }

    //overload the initial swap of ministl
    template <class _Tp1,class _Tp2>
    void swap (pair<_Tp1,_Tp2>& lhs,pair<_Tp1,_Tp2>& rhs)
    {
        lhs.swap(rhs);
    }

    template <class T1,class T2>
    pair<T1,T2> make_pair(T1 && value1,T2 && value2)
    {
        return pair<T1,T2>(ministl::forward<T1>(value1),ministl::forward<T2>(value2));
    }

}


#endif //MINISTL_UTIL_H
