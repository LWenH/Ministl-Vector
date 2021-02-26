#ifndef MINISTL_UNINITIALIZED_H
#define MINISTL_UNINITIALIZED_H

// This header is used to construct elements for the uninitialized space or memory

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace ministl
{
    /**************************************uninitialized_copy********************************************/
    /******************把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置***************/
    /***************************************************************************************************/
    template <class InputIter,class ForwardIter>
    ForwardIter unchecked_uninitialized_copy(InputIter first,InputIter last,ForwardIter result,std::true_type)
    {
        return ministl::copy(first,last,result);
    }

    template <class InputIter,class ForwardIter>
    ForwardIter unchecked_uninitialized_copy(InputIter first,InputIter last,ForwardIter result,std::false_type)
    {
        auto cur = result;
        try {
            for(;first != last;++first,++cur)
                ministl::construct(&*cur,*first);
        }
        catch (...)
        {
            for(;result != cur;++result)
                ministl::destroy(&*cur);
        }
        return cur;
    }

    template <class InputIter,class ForwardIter>
    ForwardIter uninitialized_copy(InputIter first,InputIter last,ForwardIter result)
    {
        return ministl::unchecked_uninitialized_copy(first, last, result,std::is_trivially_copy_assignable<
                typename iterator_traits<ForwardIter>::value_type>{});
    }

    /**************************************uninitialized_fill********************************************/
    /************************************把 [first, last) 上填充内容element********************************/
    /***************************************************************************************************/

    template <class ForwardIter,class T>
    void unchecked_uninitialized_fill(ForwardIter first,ForwardIter last,const T&  value,std::true_type)
    {
        ministl::fill(first,last,value);
    }

    template <class ForwardIter,class T>
    void unchecked_uninitialized_fill(ForwardIter first,ForwardIter last,const T&  value,std::false_type)
    {
        auto cur = first;
        try {
            for(; cur != last;++cur)
                ministl::construct(&*cur,value);
        }
        catch (...)
        {
            //todo may be type error here
            ministl::destroy(first,cur);
        }
    }

    template <class ForwardIter, class T>
    void  uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
    {
        ministl::unchecked_uninitialized_fill(first, last, value,std::is_trivially_copy_assignable<
                typename iterator_traits<ForwardIter>::value_type>{});
    }


    /**************************************uninitialized_fill_n*****************************************/
    /************************************在 [first,first + n)上填充内容value******************************/
    /***************************************************************************************************/
    template <class ForwardIter,class Size,class T>
    ForwardIter unchecked_uninitialized_fill_n(ForwardIter first,Size n,const T& value,std::true_type)
    {
        return ministl::fill_n(first,n,value);
    }

    template <class ForwardIter,class Size,class T>
    ForwardIter unchecked_uninitialized_fill_n(ForwardIter first,Size n,const T& value,std::false_type)
    {
        auto cur = first;
        try {
            for(;n > 0;--n,++cur)
                ministl::construct(&*cur,value);
        }
        catch (...)
        {
            ministl::destroy(first,cur);
        }
        return cur;
    }

    template <class ForwardIter, class Size, class T>
    ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
    {
        return ministl::unchecked_uninitialized_fill_n(first, n, value,std::is_trivially_copy_assignable<
        typename iterator_traits<ForwardIter>::value_type>{});
    }

    /***************************************uninitialized_move******************************************/
    /******************把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置*****************/
    /***************************************************************************************************/
    template <class InputIter,class ForwardIter>
    ForwardIter unchecked_uninitialized_move(InputIter first,InputIter last,ForwardIter result,std::true_type)
    {
        return ministl::move(first,last,result);
    }

    template <class InputIter,class ForwardIter>
    ForwardIter unchecked_uninitialized_move(InputIter first,InputIter last,ForwardIter result,std::false_type)
    {
        auto cur = result;
        try {
            for(;first != last;++first,++cur)
                ministl::construct(&*cur,ministl::move(*first));
        }
        catch(...)
        {
            ministl::destroy(result,cur);
        }
        return cur;
    }

    template <class InputIter, class ForwardIter>
    ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
    {
        return ministl::unchecked_uninitialized_move(first, last, result,std::is_trivially_move_assignable<
        typename iterator_traits<InputIter>::value_type>{});
    }

    /**************************************uninitialized_move_n*****************************************/
    /****************把[first,first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置***************/
    /***************************************************************************************************/
    template <class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
    {
        return ministl::move(first, first + n, result);
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++first, ++cur)
            {
                ministl::construct(&*cur, ministl::move(*first));
            }
        }
        catch (...)
        {
            for (; result != cur; ++result)
                ministl::destroy(&*result);
            throw;
        }
        return cur;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
    {
        return ministl::unchecked_uninitialized_move_n(first, n, result,std::is_trivially_move_assignable<
        typename iterator_traits<InputIter>::value_type>{});
    }

} //namespace ministl


#endif //MINISTL_UNINITIALIZED_H
