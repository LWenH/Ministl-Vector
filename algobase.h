#ifndef MINISTL_ALGOBASE_H
#define MINISTL_ALGOBASE_H

// This header contains the basic algorithm of ministl
// unchecked means uncheck safe status function

#include <cstring>
#include <type_traits>
#include "iterator.h"
#include "util.h"

namespace ministl
{

#ifdef max
#pragma message("#undefine marco max")
#undef max
#endif

#ifdef min
#pragma message("#undefine marco min")
#undef min
#endif


    template <class T>
    const T& max(const T& lhs,const T& rhs)
    {
        return lhs < rhs ? rhs : lhs;
    }

    template <class T>
    const T& min(const T& lhs,const T& rhs)
    {
        return lhs < rhs ? lhs : rhs;
    }

    template <class T,class Compare>
    const T& min(const T& lhs,const T& rhs,Compare cmp)
    {
        return cmp(rhs,lhs) ? rhs : lhs;
    }

    //将两个迭代器的对象swap
    template <class Iter1,class Iter2>
    void iter_swap(Iter1 first,Iter2 second)
    {
        //交换两个迭代器所指的对象
        ministl::swap(*first,*second);
    }

    /*********************************************copy****************************************************/
    /******************把[first, last)区间内的元素拷贝到[first2,first2+(last - first))内*********************/
    /****************************************************************************************************/
    template <class InputIter,class OutputIter>
    OutputIter
    unchecked_copy_cat(InputIter first,InputIter last,OutputIter first2,ministl::input_iterator_tag)
    {
        while (first != last)
        {
            *first2++ = *first++;
        }
        return first2;
    }

    template <class RandomIter,class OutputIter>
    OutputIter
    unchecked_copy_cat(RandomIter first,RandomIter last,OutputIter first2,ministl::random_access_iterator_tag)
    {
        for(auto n = last - first;n > 0; --n,++first,++first2)
        {
            *first2 = *first;
        }
        return first2;
    }

    //重载决议(overload resolution)
    template <class InputIter,class OutputIter>
    OutputIter
    unchecked_copy(InputIter first,InputIter last,OutputIter first2)
    {
        return unchecked_copy_cat(first,last,first2,ministl::iterator_category(first));
    }

    //trivially copy assignable 特化版本,即普通数组和指针类型
    template <class Tp,class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type ,Up>::value &&
            std::is_trivially_copy_assignable<Up>::value,Up*>::type
    unchecked_copy(Tp* first,Tp* last,Up* first2)
    {
        const auto n = static_cast<size_t>(last - first);
        if(n != 0)
            std::memmove(first2,first,n * sizeof(Up));
        //tail position is returned
        return first2 + n;
    }

    //interface for trivial one
    template <class InputIter,class OutputIter>
    OutputIter copy(InputIter first,InputIter last,OutputIter first2)
    {
        return unchecked_copy(first,last,first2);
    }

    /*********************************************copy_backward*******************************************/
    /*********************把[first, last)区间内的元素拷贝到[last2-(last - first),last2)内********************/
    /****************************************************************************************************/

    template <class BidirectionalIter1,class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward_cat(BidirectionalIter1 first,BidirectionalIter1 last,BidirectionalIter1 last2,
            ministl::bidirectional_iterator_tag)
    {
        while(first != last)
        {
            *last2-- = *first++;
        }
        return last2;
    }

    template <class RandomAccessIter1,class RandomAccessIter2>
    RandomAccessIter2
    unchecked_copy_backward_cat(RandomAccessIter1 first,RandomAccessIter1 last,RandomAccessIter1 last2,
            ministl::random_access_iterator_tag)
    {
        for(auto n = last - first;n > 0;--n)
        {
            //todo judge prefix or postfix
            *last2-- = *first--;
        }
        return last2;
    }

    //overload resolution
    template <class BidirectionalIter1,class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward(BidirectionalIter1 first,BidirectionalIter1 last,BidirectionalIter1 last2)
    {
        unchecked_copy_backward_cat(first,last,last2,ministl::iterator_category(first));
    }

    template <class _Tp1,class _Tp2>
    typename std::enable_if<
            std::is_same<typename std::remove_const<_Tp1>::type ,_Tp2>::value &&
            std::is_trivially_copy_assignable<_Tp2>::value,_Tp2*>::type
    unchecked_copy_backward(_Tp1* first,_Tp1* last,_Tp2* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if(n != 0)
        {
            result -= n;
            std::memmove(result,first, n * sizeof(_Tp2));
        }
        return result;
    }

    template <class InputIter,class OutputIter>
    OutputIter copy_backward(InputIter first,InputIter last,OutputIter result)
    {
        return unchecked_copy_backward(first,last,result);
    }

    /**************************************************copy_if**********************************************/
    /*****************把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上****************/
    /*******************************************************************************************************/

    template <class InputIter,class OutputIter,class UnaryPredicate>
    OutputIter copy_if(InputIter first,InputIter last,OutputIter result,UnaryPredicate predicate)
    {
        for(;first != last;++first)
        {
            if(predicate(*first))
            {
                *result++ = *first;
            }
        }
        return result;
    }

    /**************************************************copy_n***********************************************/
    /***************把[first, first + n)的元素拷贝到以 result 为起始的位置上,返回一个指向尾部的pair*****************/
    /*******************************************************************************************************/
    template <class InputIter,class Size,class OutputIter>
    OutputIter unchecked_copy_n(InputIter first,Size n,OutputIter result,ministl::input_iterator_tag)
    {
        for(;n > 0;--n,++first,++result)
            *result = *first;
        return ministl::pair<InputIter,OutputIter>(first,result);
    }

    template <class RandomIter,class Size,class OutputIter>
    OutputIter unchecked_copy_n(RandomIter first,Size n,OutputIter result,ministl::random_access_iterator_tag)
    {
        auto last = first + n;
        return ministl::pair<RandomIter,OutputIter>(last,ministl::copy(first,result));
    }

    template <class InputIter,class Size,class OutputIter>
    OutputIter copy_n(InputIter first,Size n,OutputIter result)
    {
        return ministl::unchecked_copy_n(first,n,result,ministl::iterator_category(first));
    }

    /***************************************************move************************************************/
    /**************************把[first, last)的元素move到以 result 为起始的位置上******************************/
    /******************************************************************************************************/
    template <class InputIter, class OutputIter>
    OutputIter
    unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
                       ministl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result)
        {
            *result = ministl::move(*first);
        }  return result;
    }

    template <class RandomIter, class OutputIter>
    OutputIter
    unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result,
                       ministl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = ministl::move(*first);
        }
        return result;
    }

    template <class InputIter, class OutputIter>
    OutputIter
    unchecked_move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move_cat(first, last, result, iterator_category(first));
    }

// 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
            std::is_trivially_move_assignable<Up>::value,
            Up*>::type
    unchecked_move(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
            std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move(first, last, result);
    }

    /********************************************move_backward*******************************************/
    /******************将 [first, last)区间内的元素移动到 [result - (last - first), result)内***************/
    /***************************************************************************************************/

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, ministl::bidirectional_iterator_tag)
    {
        while (first != last)
            *--result = ministl::move(*--last);
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomIter1, class RandomIter2>
    RandomIter2
    unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
                                RandomIter2 result, ministl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n)
            *--result = ministl::move(*--last);
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result)
    {
        return unchecked_move_backward_cat(first, last, result,
                                           iterator_category(first));
    }

    // 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
            std::is_trivially_move_assignable<Up>::value,
            Up*>::type
    unchecked_move_backward(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_move_backward(first, last, result);
    }

    /**************************************************equal************************************************/
    /************************判断[first, last)的元素是否与 [result,result + last - first)完全相等***************/
    /*******************************************************************************************************/
    template <class InputIter1,class InputIter2>
    bool equal(InputIter1 first1,InputIter1 last,InputIter2 first2)
    {
        while(first1 != last)
            if(*first1 != *last) return false;
        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIter1, class InputIter2, class Compared>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (!comp(*first1, *first2))
                return false;
        }
        return true;
    }

    /**************************************************fill_n***********************************************/
    /******************************************从first开始填充n个值*******************************************/
    /*******************************************************************************************************/

    template <class OutputIter, class Size, class T>
    OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value)
    {
        for (; n > 0; --n, ++first)
        {
            *first = value;
        }
        return first;
    }

    // 为 one-byte 类型提供特化版本
    template <class Tp, class Size, class Up>
    typename std::enable_if<
            std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
            !std::is_same<Tp, bool>::value &&
            std::is_integral<Up>::value && sizeof(Up) == 1,
            Tp*>::type
    unchecked_fill_n(Tp* first, Size n, Up value)
    {
        if (n > 0)
        {
            std::memset(first, (unsigned char)value, (size_t)(n));
        }
        return first + n;
    }

    template <class OutputIter, class Size, class T>
    OutputIter fill_n(OutputIter first, Size n, const T& value)
    {
        return unchecked_fill_n(first, n, value);
    }


    /***************************************************fill************************************************/
    /******************************************填充[first,last)的值******************************************/
    /*******************************************************************************************************/
    template <class ForwardIter,class T>
    void fill_cat(ForwardIter first,ForwardIter last,const T& value,ministl::forward_iterator_tag)
    {
        for(;first != last;++first)
            *first = value;
    }

    template <class RandomIter,class T>
    void fill_cat(RandomIter first,RandomIter last,const T& value,ministl::random_access_iterator_tag)
    {
        ministl::fill_n(first,last-first,value);
    }

    template <class ForwardIter,class T>
    void fill(ForwardIter first,ForwardIter last, const T& value)
    {
        ministl::fill_cat(first,last,value,ministl::iterator_category(first));
    }


    /*******************************************lexicographical_compare************************************/
    // lexicographical_compare
    // 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
    // (1)如果第一序列的元素较小，返回 true ，否则返回 false
    // (2)如果到达 last1 而尚未到达 last2 返回 true
    // (3)如果到达 last2 而尚未到达 last1 返回 false
    // (4)如果同时到达 last1 和 last2 返回 false
    /*******************************************************************************************************/
    template <class InputIter1,class InputIter2>
    bool lexicographical_compare(InputIter1 first1,InputIter1 last1,InputIter2 first2,InputIter2 last2)
    {
        for(;first1 != last1 && first2 != last2;++first1,++first2)
        {
            if(*first1 < *first2) return true;
            if(*first2 < *first1) return false;
        }
        return first1 == last1 && first2 != last2;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIter1, class InputIter2, class Compare>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                 InputIter2 first2, InputIter2 last2, Compare comp)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (comp(*first1, *first2))
                return true;
            if (comp(*first2, *first1))
                return false;
        }
        return first1 == last1 && first2 != last2;
    }

    // unsigned char version
    bool lexicographical_compare(const unsigned char* first1,const unsigned char*last1,
            const unsigned char* first2,const unsigned char* last2)
    {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        const auto res = std::memcmp(first1,first2,ministl::min(len1,len2));
        return res != 0 ? res < 0 : len1 < len2;
    }

    /*************************************************mismatch**********************************************/
    /****************平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素*****************/
    /*******************************************************************************************************/
    template <class InputIter1,class InputIter2>
    ministl::pair<InputIter1,InputIter2>
    mismatch(InputIter1 first1,InputIter1 last1,InputIter2 first2)
    {
        while (first1 != last1 && *first1++ == *first2++);
        return ministl::pair<InputIter1,InputIter2>(first1,first2);
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIter1, class InputIter2, class Compared>
    ministl::pair<InputIter1, InputIter2>
    mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp)
    {
        while (first1 != last1 && comp(*first1, *first2))
        {
            ++first1;
            ++first2;
        }
        return ministl::pair<InputIter1, InputIter2>(first1, first2);
    }




}//namespace ministl



#endif //MINISTL_ALGOBASE_H
