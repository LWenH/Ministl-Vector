#ifndef MINISTL_ITERATOR_H
#define MINISTL_ITERATOR_H
#include <cstddef>
#include <type_traits>
#include "type_traits.h"

namespace ministl
{
    //五种迭代器类型(iterator_category)
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag{};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag{};

    //迭代器模版
    template <class Category,class T,class Distance = ptrdiff_t ,class Pointer = T*,class Reference=T&>
    struct iterator
    {
        typedef Category            iterator_category;
        typedef T                   value_type;
        typedef Pointer             pointer;
        typedef Reference           reference;
        typedef Distance            difference_type;
    };

    template <class T>
    class has_iterator_category
    {
    private:
        struct binary{char a;char b;};
        template <class U> static binary Test(...){}
        template <class U> static char Test(typename U::iterator_category *arg = 0){}
    public:
        //单双元判断,sizeof在编译时期确定,typeid在runtime确定
        static const bool value = sizeof(Test<T>(0)) == sizeof(char);
    };

    //implementation的泛化原型
    template <class Iterator, bool>
    struct iterator_traits_impl {};

    //implement偏特化
    template <class Iterator>
    struct iterator_traits_impl<Iterator,true>
    {
        typedef typename Iterator::iterator_category     iterator_category;
        typedef typename Iterator::T                     value_type;
        typedef typename Iterator::Pointer               pointer;
        typedef typename Iterator::Reference             reference;
        typedef typename Iterator::Distance              difference_type;
    };

    //特性萃取helper(泛化原型),带fill in 结构
    template <class Iterator, bool>
    struct iterator_traits_helper {};

    //Valid helper,flag由是否可转换迭代器的类型决定
    template <class Iterator>
    struct iterator_traits_helper<Iterator,true> : public iterator_traits_impl<Iterator,
            std::is_convertible<typename Iterator::iterator_category,input_iterator_tag>::value ||
            std::is_convertible<typename Iterator::iterator_category,output_iterator_tag>::value>
    {
    };

    //萃取迭代器的特性
    template <class Iterator>
    struct iterator_traits : public iterator_traits_helper<Iterator,has_iterator_category<Iterator>::value>
    {
    };

    //原生指针偏特化版本
    template <class T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag          iterator_category;
        typedef T                                   value_type;
        typedef T*                                  pointer;
        typedef T&                                  reference;
        typedef ptrdiff_t                           difference_type;
    };

    //原生const_value指针偏特化
    template <class T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag          iterator_category;
        typedef const T                             value_type;
        typedef const T*                            pointer;
        typedef const T&                            reference;
        typedef ptrdiff_t                           difference_type;
    };

    //可以转化的话，无论隐式还是显式，都可以认为value为true，亦即iterator_category为所对应的类别
    template <class T,class U,bool = has_iterator_category<iterator_traits<T>>::value>
    struct has_iterator_category_of : public m_bool_constant <
            std::is_convertible<typename iterator_traits<T>::iterator_category ,U>::value>
    {
    };

    //其他类型迭代器的无效偏特化
    template <class T,class U>
    struct has_iterator_category_of<T,U,false>{};

    //萃取5种属性的迭代器
    template <class Iterator>
    struct is_input_iterator : public has_iterator_category_of<Iterator,input_iterator_tag>{};

    template <class Iterator>
    struct is_output_iterator : public has_iterator_category_of<Iterator,output_iterator_tag>{};

    template <class Iterator>
    struct is_forward_iterator : public has_iterator_category_of<Iterator,forward_iterator_tag>{};

    template <class Iterator>
    struct is_bidirectional_iterator : public has_iterator_category_of<Iterator,bidirectional_iterator_tag>{};
    
    template <class Iterator>
    struct is_random_access_iterator : public has_iterator_category_of<Iterator,random_access_iterator_tag>{};

    //萃取某迭代器的Category
    template <class Iterator>
    typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&)
    {
        return typename iterator_traits<Iterator>::iterator_category();
    }

    //萃取某个迭代器的difference_type
    template <class Iterator>
    typename iterator_traits<Iterator>::difference_type *
    difference_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(NULL);
    }

    //萃取某个迭代器的value_type
    template <class Iterator>
    typename iterator_traits<Iterator>::value_type
    value_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type>(0);
    }

    /***************************************计算迭代器的距离****************************************/
    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first,const InputIterator& last,input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while(first != last)
        {
            ++n;
            ++first;
        }
        return n;
    }

    template <class RandomIterator>
    typename iterator_traits<RandomIterator>::difference_type
    distance(const RandomIterator& first,const RandomIterator& last,random_access_iterator_tag)
    {
        return last - first;
    }

    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first,InputIterator last)
    {
        return distance(first,last,iterator_category(first));
    }

    /*******************************************************************************************/

    /***************************************迭代器向前移动n个单位**********************************/
    template <class InputIterator,class Distance>
    void advance_dispatch(InputIterator &pos,Distance n,input_iterator_tag)
    {
        while (n--)
            ++pos;
    }

    template <class RandomIterator,class Distance>
    void advance_dispatch(RandomIterator& pos,Distance n,random_access_iterator_tag)
    {
        pos += n;
    }

    template <class BidirectionalIterator ,class Distance>
    void advance_dispatch(BidirectionalIterator &pos,Distance n,bidirectional_iterator_tag)
    {
        if(n >= 0)
            while (n--) ++pos;
        else
            while (n++) --pos;
    }

    //重载决议,提供上层interface,这里选用哪个会在编译期决定
    template <class InputIterator, class Distance>
    void advance(InputIterator& pos, Distance n)
    {
        advance_dispatch(pos, n, iterator_category(pos));
    }

    /*****************************************************************************************/

    //反向迭代器模版
    template <class Iterator>
    class reverse_iterator
    {
    private:
        Iterator current;       //记录对应的正向迭代器
    public:
        typedef typename iterator_traits<Iterator>::iterator_category           iterator_category;
        typedef typename iterator_traits<Iterator>::value_type                  value_type;
        typedef typename iterator_traits<Iterator>::pointer                     pointer;
        typedef typename iterator_traits<Iterator>::reference                   reference;
        typedef typename iterator_traits<Iterator>::difference_type             difference_type;
        typedef Iterator                                                        iterator_type;
        typedef reverse_iterator<Iterator>                                      self;

    public:
        reverse_iterator()= default;
        explicit reverse_iterator(iterator_type i): current(i){}
        reverse_iterator(const self& other) : current(other.current){}

    public:
        //取出正向迭代器
        iterator_type base() const
        {
            return current;
        }

        reference operator*() const
        {
            //对应前向迭代器的前一个位置,array index start from zero
            auto tmp = current;
            return *--tmp;
        }

        pointer operator->() const
        {
            return &(--current);
        }

        self& operator++()
        {
            --current;
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            --current;
            return tmp;
        }

        self& operator--()
        {
            ++current;
            return *this;
        }

        self operator--(int)
        {
            self tmp =  *this;
            ++current;
            return tmp;
        }

        self& operator+= (difference_type n)
        {
            current -= n;
            return *this;
        }

        self operator+(difference_type n) const
        {
            return self(current - n);
        }

        self operator-(difference_type n) const
        {
            return self(current + n);
        }

        self& operator-= (difference_type n)
        {
            current += n;
            return *this;
        }

        reference operator[](difference_type n) const
        {
            //取出特定位置的current base
            return *(*this + n);
        }
    };

    //out of class definition,global utilized
    template <class Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator - (const reverse_iterator<Iterator>& first,
            const reverse_iterator<Iterator>& second)
    {
        return first.base() - second.base();
    }

    template <class Iterator>
    bool operator == (const reverse_iterator<Iterator>& first,
            const reverse_iterator<Iterator>& second)
    {
        return first.base() == second.base();
    }

    template <class Iterator>
    bool operator < (const reverse_iterator<Iterator>& first,
            const reverse_iterator<Iterator>& second)
    {
        return first.base() < second.base();
    }

    template <class Iterator>
    bool operator != (const reverse_iterator<Iterator>& first,
            const reverse_iterator<Iterator>& second)
    {
        return !(first == second);
    }

    template <class Iterator>
    bool operator > (const reverse_iterator<Iterator>& first,
            const reverse_iterator<Iterator>& second)
    {
        return second < first;
    }

    template <class Iterator>
    bool operator >= (const reverse_iterator<Iterator>& first,
            const reverse_iterator<Iterator>& second)
    {
        return !(first < second);
    }

    template <class Iterator>
    bool operator <= (const reverse_iterator<Iterator>& first,
            const reverse_iterator<Iterator>& second)
    {
        return !(first > second);
    }

}
#endif //MINISTL_ITERATOR_H
