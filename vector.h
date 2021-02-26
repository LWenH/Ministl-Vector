#ifndef MINISTL_VECTOR_H
#define MINISTL_VECTOR_H

// 这个头文件包含一个模板类 vector
// vector : 向量

// notes:
// 异常保证：
//   ministl::vector<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
//   * emplace
//   * emplace_back
//   * push_back
//   当 std::is_nothrow_move_assignable<T>::value == true 时，以下函数也满足强异常保证：
//   * reserve
//   * resize
//   * insert

#include "iterator.h"
#include "exception.h"
#include "util.h"
#include "memory.h"
#include "type_traits.h"
#include <initializer_list>
#include <limits>

namespace ministl
{
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

    /***********************************************vector******************************************************/

    template <class T>
    class vector
    {
        //vector<bool>[]返回是一个proxy class,包含了对bool的封装，此处不实现
        static_assert(!std::is_same<bool ,T>::value,"bool in vector is abandoned in ministl");
    public:
        // vector 的嵌套型别定义
        typedef typename ministl::allocator<T>                          allocator_type;
        typedef typename ministl::allocator<T>                          data_allocator;

        typedef typename allocator_type::value_type                     value_type;
        typedef typename allocator_type::pointer                        pointer;
        typedef typename allocator_type::const_pointer                  const_pointer;
        typedef typename allocator_type::reference                      reference;
        typedef typename allocator_type::const_reference                const_reference;
        typedef typename allocator_type::size_type                      size_type;
        typedef typename allocator_type::difference_type                difference_type;

        typedef value_type*                                             iterator;
        typedef const value_type*                                       const_iterator;
        typedef ministl::reverse_iterator<iterator>                     reverse_iterator;
        typedef ministl::reverse_iterator<const_iterator>               const_reverse_iterator;

        allocator_type get_allocator() { return data_allocator();}

    private:
        iterator begin_;        //目前使用空间头部
        iterator end_;          //目前使用空间尾部
        iterator cap_;          //目前使用空间尾部

    public:
        vector() noexcept
        { try_init(); }

        explicit vector(size_type n)
        { fill_init(n,value_type()); }

        vector(size_type n,const value_type& value)
        { fill_init(n,value);}

        //范围初始化
        template <class Iter, typename std::enable_if<ministl::is_input_iterator<Iter>::value,int>::type = 0>
        vector(Iter first,Iter last)
        {
            MINISTL_DEBUG(!(last < first));
            range_init(first,last);
        }

        vector(const vector& other)
        {
            range_init(other.begin_,other.end_);
        }

        vector(vector && other) noexcept : begin_(other.begin_),end_(other.end_),cap_(other.cap_)
        {
            other.begin_ = nullptr;
            other.end_ = nullptr;
            other.cap_ = nullptr;
        }

        vector(std::initializer_list<value_type> list)
        {
            range_init(list.begin(),list.end());
        }

        vector& operator= (const vector & other);
        vector& operator= (vector && other) noexcept ;

        vector& operator=(std::initializer_list<value_type > list)
        {
            vector tmp(list.begin(),list.end());
            //与另一个vector交换
            swap(tmp);
            return *this;
        }

        ~vector(){
            destroy_and_recover(begin_,end_,cap_ - begin_);
            begin_ = end_ = cap_ = nullptr;
        }

    public:
        /****************************************迭代器位置相关函数***************************************/
        iterator begin() noexcept{
            return begin_;
        }

        //for const function invoke
        iterator begin() const noexcept{
            return begin_;
        }

        iterator end() noexcept{
            return end_;
        }

        iterator end() const noexcept{
            return end_;
        }

        reverse_iterator rbegin() noexcept{
            return reverse_iterator(end());
        }

        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        const_iterator cbegin() const noexcept {
            return begin();
        }

        const_iterator cend() const noexcept {
            return end();
        }

        const_reverse_iterator crbegin() const noexcept{
            return rbegin();
        }

        const_reverse_iterator crend() const noexcept{
            return rend();
        }
        /************************************************************************************************/

        //容量相关操作
        bool empty() const noexcept{
            return begin_ == end_;
        }

        size_type size() const noexcept {
            return static_cast<size_type> (end_ - begin_);
        }

        size_type max_size() const noexcept {
            //获取系统size_t最大值
            return std::numeric_limits<size_t >::max() / sizeof(T);
        }

        size_type capacity() const noexcept
        {
            return static_cast<size_type>(cap_ - begin_);
        }

        void reserve(size_type n);
        void shrink_to_fit();

        //访问元素
        reference operator[](size_type n)
        {
            MINISTL_DEBUG(n <= size());
            return *(begin_ + n);
        }

        const_reference operator[](size_type n) const
        {
            MINISTL_DEBUG(n <= size());
            return *(begin_ + n);
        }

        reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(n >= size(), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        const_reference at(size_type n) const
        {
            THROW_OUT_OF_RANGE_IF(n >= size(), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        reference front()
        {
            MINISTL_DEBUG(!empty());
            return *begin_;
        }

        const_reference front() const
        {
            MINISTL_DEBUG(!empty());
            return *begin_;
        }

        reference back()
        {
            MINISTL_DEBUG(!empty());
            return *(end_ - 1);
        }

        const_reference back() const
        {
            MINISTL_DEBUG(!empty());
            return *(end_ - 1);
        }

        pointer data()              noexcept { return begin_;}
        const_pointer data()  const noexcept { return begin_;}

        //修改容器相关操作
        //assign
        void assign(size_type n,const value_type& value)
        {
            fill_assign(n,value);
        }

        template <class Iter,typename std::enable_if<
                ministl::is_input_iterator<Iter>::value,int>::type = 0>
        void assign(Iter first,Iter last)
        {
            MINISTL_DEBUG((first < last));
            copy_assign(first,last,iterator_category(first));
        }

        void assign(std::initializer_list<value_type > list)
        {
            copy_assign(list.begin(),list.end(),ministl::forward_iterator_tag());
        }

        //emplace
        template <class ...Args>
        iterator emplace(const_iterator pos,Args&& ...args);

        template <class ...Args>
        void emplace_back(Args&& ...args);

        //push_back
        void push_back(const value_type& value);
        void push_back(value_type && value)
        {emplace_back(value);}

        void pop_back();

        //insert
        iterator insert(const_iterator pos, const value_type& value);
        iterator insert(const_iterator pos, value_type&& value)
        { return emplace(pos, ministl::move(value)); }

        iterator insert(const_iterator pos,size_type n,const value_type& value)
        {
            MINISTL_DEBUG(pos >= begin() && pos <= end());
            return fill_insert(const_cast<iterator>(pos),n,value);
        }

        template <class Iter,typename std::enable_if<
                ministl::is_input_iterator<Iter>::value,int>::type = 0>
        void insert(const_iterator pos,Iter first,Iter last)
        {
            MINISTL_DEBUG(pos >= begin() && pos<= end() && !(last < first));
            copy_insert(const_cast<iterator>(pos),first,last);
        }

        //erase / clear
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first,const_iterator last);
        void clear()    { erase(begin(),end());}

        //resize / reverse
        void resize(size_type new_size) { return resize(new_size,value_type());}
        void resize(size_type new_size,const value_type&);

//        void reverse(){ministl::reverse(begin(),end());}

        void swap(vector &rhs) noexcept;

    private:
        void try_init() noexcept;

        void init_space(size_type size,size_type cap);
        void fill_init(size_type n,const value_type& value);

        template <class Iter>
        void range_init(Iter first,Iter last);

        void destroy_and_recover(iterator first,iterator last,size_type n);

        //get growth size
        size_type get_new_cap(size_type add_size);

        // assign

        void      fill_assign(size_type n, const value_type& value);

        template <class IIter>
        void      copy_assign(IIter first, IIter last, input_iterator_tag);

        template <class FIter>
        void      copy_assign(FIter first, FIter last, forward_iterator_tag);

        // reallocate

        template <class... Args>
        void      reallocate_emplace(iterator pos, Args&& ...args);
        void      reallocate_insert(iterator pos, const value_type& value);

        // insert

        iterator  fill_insert(iterator pos, size_type n, const value_type& value);
        template <class IIter>
        void      copy_insert(iterator pos, IIter first, IIter last);

        // shrink_to_fit

        void      reinsert(size_type size);

    };

    /***********************************************implementation********************************************************/

    template <class T>
    vector<T>& vector<T>::operator=(const vector &other)
    {
        if(this != &other)
        {
            const auto len = other.size();
            //当前分配的容量不足
            if(len > capacity())
            {
                vector tmp(other.begin(),other.end());
                swap(tmp);
            }
            else if(size() >= len)
            {
                auto i = ministl::copy(other.begin(),other.end(),begin());
                data_allocator::destroy(i,end_);
                end_ = begin_ + len;
            }
            else
            {
                //分两段copy,一段是已经申请过的，另一段是未申请的
                ministl::copy(other.begin(), other.begin() + size(), begin_);
                ministl::uninitialized_copy(other.begin() + size(), other.end(), end_);
                cap_ = end_ = begin_ + len;
            }
        }
        return *this;
    }

    template <class T>
    vector<T>& vector<T>::operator= (vector &&other) noexcept
    {
        destroy_and_recover(begin_,end_,cap_ - begin_);
        begin_ = other.begin_;
        end_ = other.end_;
        cap_ = other.cap_;
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.cap_ = nullptr;
        return *this;
    }

    //预留空间大小,当原容量小于要求大小时,才会重新分配
    template <class T>
    void vector<T>::reserve(size_type n) {
        if(capacity() < n)
        {
            THROW_LENGTH_ERROR_IF(n > max_size(),"n can not larger than max_size() in vector<T>::reserve(n)");
            const size_type old_size = size();
            auto tmp = data_allocator::allocate(n);
            ministl::uninitialized_move(begin_,end_,tmp);
            data_allocator::deallocate(begin_,cap_ - begin_);
            begin_ = tmp;
            end_ = tmp + old_size;
            cap_ = tmp + n;
        }
    }

    //放弃多余容量
    template <class T>
    void vector<T>::shrink_to_fit() {
        if(end_ < cap_)
            reinsert(size());
    }

    // 在 pos 位置就地构造元素，避免额外的复制或移动开销
    template <class T>
    template <class ...Args>
    typename vector<T>::iterator
    vector<T>::emplace(const_iterator pos, Args&& ...args)
    {
        MINISTL_DEBUG(pos >= begin() && pos <= end());
        auto casted_pos = const_cast<iterator>(pos);
        const size_type n = casted_pos - begin_;
        if (end_ != cap_ && casted_pos == end_)
        {
            //end往后移一个
            data_allocator::construct(ministl::address_of(*end_), ministl::forward<Args>(args)...);
            ++end_;
        }
        else if(end_ != cap_)
        {
            data_allocator::construct(ministl::address_of(*end_), *(end_ - 1));
            //整体往后移一个单位
            ministl::copy_backward(casted_pos,end_ - 1,end_);
            *casted_pos = value_type(ministl::forward<Args>(args)...);
        }
        else
        {
            reallocate_emplace(casted_pos, ministl::forward<Args>(args)...);
        }
        return begin_ + n;
    }


    // 在尾部就地构造元素，避免额外的复制或移动开销
    template <class T>
    template <class ...Args>
    void vector<T>::emplace_back(Args &&... args)
    {
        if(end_ < cap_)
        {
            data_allocator::construct(ministl::address_of(*end_), ministl::forward<Args>(args)...);
            ++end_;
        }
        else
        {
            reallocate_emplace(end_, ministl::forward<Args>(args)...);
        }

    }

    //push_back
    template <class T>
    void vector<T>::push_back(const value_type &value)
    {
        if(end_ != cap_)
        {
            data_allocator::construct(address_of(*end_),value);
            ++end_;
        }
        else
        {
            reallocate_insert(end_,value);
        }
    }

    //pop_back
    template <class T>
    void vector<T>::pop_back()
    {
        MINISTL_DEBUG(!empty());
        data_allocator::destroy(end_-1);
        --end_;
    }

    template <class T>
    typename vector<T>::iterator
    vector<T>::insert(const_iterator pos, const value_type& value) {
        MINISTL_DEBUG(pos >= begin() && pos <= end());
        auto xpos = const_cast<iterator>(pos);
        const size_type n = xpos - begin_;
        if(end_ != cap_ && xpos == end_)
        {
            data_allocator::construct(ministl::address_of(*end_),value);
            ++end_;
        }
        else if(end_ != cap_)
        {
            data_allocator::construct(ministl::address_of(*end_),*(end_ - 1));
            auto copy_value = value;
            ministl::copy_backward(xpos,end_-1,end_);
            *xpos = ministl::move(copy_value);
        }
        else
        {
            reallocate_insert(xpos, value);
        }
        return begin_ + n;
    }

    // 删除 pos 位置上的元素
    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator pos)
    {
        MINISTL_DEBUG(pos >= begin() && pos < end());
        iterator xpos = begin_ + (pos - begin());
        ministl::move(xpos + 1, end_, xpos);
        data_allocator::destroy(end_ - 1);
        --end_;
        return xpos;
    }

    // 删除[first, last)上的元素
    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator first, const_iterator last)
    {
        MINISTL_DEBUG(first >= begin() && last <= end() && !(last < first));
        const auto n = first - begin();
        iterator r = begin_ + (first - begin());
        //move完要析构
        data_allocator::destroy(ministl::move(r + (last - first), end_, r), end_);
        end_ = end_ - (last - first);
        return begin_ + n;
    }

    template <class T>
    void vector<T>::resize(size_type new_size, const value_type& value)
    {
        if(new_size < size())
        {
            erase(begin() + new_size,end());
        }
        else
        {
            insert(end(),new_size - size(),value);
        }
    }

    // 与另一个 vector 交换
    template <class T>
    void vector<T>::swap(vector<T>& rhs) noexcept
    {
        if (this != &rhs)
        {
            ministl::swap(begin_, rhs.begin_);
            ministl::swap(end_, rhs.end_);
            ministl::swap(cap_, rhs.cap_);
        }
    }

    /*****************************************************************************************/
    // helper function
    // try_init 函数，若分配失败则忽略，不抛出异常

    template <class T>
    void vector<T>::try_init() noexcept
    {
        try {
            begin_ = data_allocator::allocate(16);
            end_ = begin_;
            cap_ = begin_ + 16;
        }
        catch (...) {
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
        }
    }

    // init_space 函数
    template <class T>
    void vector<T>::init_space(size_type size, size_type cap)
    {
        try
        {
            begin_ = data_allocator::allocate(cap);
            end_ = begin_ + size;
            cap_ = begin_ + cap;
        }
        catch (...)
        {
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
            throw;
        }
    }

    // fill_init 函数
    template <class T>
    void vector<T>::
    fill_init(size_type n, const value_type& value)
    {
        //16个起分配
        const size_type init_size = ministl::max(static_cast<size_type>(16), n);
        init_space(n, init_size);
        ministl::uninitialized_fill_n(begin_, n, value);
    }

    // range_init 函数
    template <class T>
    template <class Iter>
    void vector<T>::
    range_init(Iter first, Iter last)
    {
        const size_type init_size = ministl::max(static_cast<size_type>(last - first),
                                               static_cast<size_type>(16));
        init_space(static_cast<size_type>(last - first), init_size);
        ministl::uninitialized_copy(first, last, begin_);
    }

    // destroy_and_recover 函数
    template <class T>
    void vector<T>::
    destroy_and_recover(iterator first, iterator last, size_type n)
    {
        data_allocator::destroy(first, last);
        data_allocator::deallocate(first, n);
    }

    // get_new_cap 函数
    template <class T>
    typename vector<T>::size_type
    vector<T>::
    get_new_cap(size_type boom_size)
    {
        const auto old_size = capacity();
        THROW_LENGTH_ERROR_IF(old_size > max_size() - boom_size,
                              "vector<T>'s size too big");
        // old size > 2/3 max size
        if (old_size > max_size() - old_size / 2)
        {
            //最小以16个多分配
            return old_size + boom_size > max_size() - 16
                   ? old_size + boom_size : old_size + boom_size + 16;
        }
        const size_type new_size = old_size == 0
                                   ? ministl::max(boom_size, static_cast<size_type>(16))
                                   : ministl::max(old_size + old_size / 2, old_size + boom_size);
        return new_size;
    }

    // fill_assign 函数
    template <class T>
    void vector<T>::
    fill_assign(size_type n, const value_type& value)
    {
        if (n > capacity())
        {
            vector tmp(n, value);
            swap(tmp);
        }
        else if (n > size())
        {
            ministl::fill(begin(), end(), value);
            end_ = ministl::uninitialized_fill_n(end_, n - size(), value);
        }
        else
        {
            erase(ministl::fill_n(begin_, n, value), end_);
        }
    }

    template <class T>
    template <class Iter>
    void vector<T>::
    copy_assign(Iter first, Iter last, ministl::input_iterator_tag)
    {
        auto cur = begin_;
        for (; first != last && cur != end_; ++first, ++cur)
        {
            *cur = *first;
        }
        if (first == last)
        {
            erase(cur, end_);
        }
        else
        {
            insert(end_, first, last);
        }
    }

    // 用 [first, last) 为容器赋值
    template <class T>
    template <class FIter>
    void vector<T>::
    copy_assign(FIter first, FIter last, forward_iterator_tag)
    {
        const size_type len = ministl::distance(first, last);
        if (len > capacity())
        {
            vector tmp(first, last);
            swap(tmp);
        }
        else if (size() >= len)
        {
            auto new_end = ministl::copy(first, last, begin_);
            data_allocator::destroy(new_end, end_);
            end_ = new_end;
        }
        else
        {
            //比size大,比cap小
            auto mid = first;
            ministl::advance(mid, size());
            //前size端
            ministl::copy(first, mid, begin_);
            //后cap - size段
            auto new_end = ministl::uninitialized_copy(mid, last, end_);
            end_ = new_end;
        }
    }

    // 重新分配空间并在 pos 处就地构造元素
    template <class T>
    template <class ...Args>
    void vector<T>::
    reallocate_emplace(iterator pos, Args&& ...args)
    {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try
        {
            new_end = ministl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(ministl::address_of(*new_end), ministl::forward<Args>(args)...);
            ++new_end;
            new_end = ministl::uninitialized_move(pos, end_, new_end);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    // 重新分配空间并在 pos 处插入元素
    template <class T>
    void vector<T>::reallocate_insert(iterator pos, const value_type& value)
    {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        const value_type& value_copy = value;
        try
        {
            new_end = ministl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(ministl::address_of(*new_end), value_copy);
            ++new_end;
            new_end = ministl::uninitialized_move(pos, end_, new_end);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    // fill_insert 函数
    template <class T>
    typename vector<T>::iterator
    vector<T>::
    fill_insert(iterator pos, size_type n, const value_type& value)
    {
        if(n == 0)
            return pos;
        const size_type xpos = pos - begin_;
        const value_type value_copy = value; //避免被覆盖
        if (static_cast<size_type>(cap_ - end_) >= n)
        { // 如果备用空间大于等于增加的空间
            const size_type after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n)
            {
                ministl::uninitialized_copy(end_ - n, end_, end_);
                end_ += n;
                ministl::move_backward(pos, old_end - n, old_end);
                ministl::uninitialized_fill_n(pos, n, value_copy);
            }
            else
            {
                end_ = ministl::uninitialized_fill_n(end_, n - after_elems, value_copy);
                end_ = ministl::uninitialized_move(pos, old_end, end_);
                ministl::uninitialized_fill_n(pos, after_elems, value_copy);
            }
        }
        else
        { // 如果备用空间不足
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try
            {
                new_end = ministl::uninitialized_move(begin_, pos, new_begin);
                new_end = ministl::uninitialized_fill_n(new_end, n, value);
                new_end = ministl::uninitialized_move(pos, end_, new_end);
            }
            catch (...)
            {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = begin_ + new_size;
        }
        return begin_ + xpos;
    }

    template <class T>
    template <class IIter>
    void vector<T>::copy_insert(iterator pos, IIter first, IIter last)
    {
        if(first == last)
            return;
        const auto n = distance(first,last);
        if((cap_ - end_) >= n)
        {
            //备用空间足够
            const auto after_elems = end_ - pos;
            auto old_end = end_;
            if(after_elems > n)
            {
                end_ = ministl::uninitialized_copy(end_ - n, end_, end_);
                ministl::move_backward(pos, old_end - n, old_end);
                ministl::uninitialized_copy(first, last, pos);
            }
            else
            {
                auto mid = first;
                ministl::advance(mid, after_elems);
                end_ = ministl::uninitialized_copy(mid, last, end_);
                end_ = ministl::uninitialized_move(pos, old_end, end_);
                ministl::uninitialized_copy(first, mid, pos);
            }
        }
        else
        { // 备用空间不足
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try
            {
                new_end = ministl::uninitialized_move(begin_, pos, new_begin);
                new_end = ministl::uninitialized_copy(first, last, new_end);
                new_end = ministl::uninitialized_move(pos, end_, new_end);
            }
            catch (...)
            {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = begin_ + new_size;
        }
    }

    // reinsert 函数
    template <class T>
    void vector<T>::reinsert(size_type size)
    {
        auto new_begin = data_allocator::allocate(size);
        try
        {
            ministl::uninitialized_move(begin_, end_, new_begin);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = begin_ + size;
        cap_ = begin_ + size;
    }

    //overload
    template <class T>
    bool operator==(const vector<T>& lhs,const vector<T>& rhs)
    {
        return ministl::lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(),rhs.end());
    }

    template <class T>
    bool operator < (const vector<T>& lhs,const vector<T>& rhs)
    {
        return ministl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), lhs.end());
    }

    template <class T>
    bool operator!=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    bool operator>(const vector<T>& lhs, const vector<T>& rhs)
    {
        return rhs < lhs;
    }

    template <class T>
    bool operator<=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T>
    bool operator>=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs < rhs);
    }

    // 重载 ministl 的 swap
    template <class T>
    void swap(vector<T>& lhs, vector<T>& rhs)
    {
        lhs.swap(rhs);
    }



}






#endif //MINISTL_VECTOR_H
