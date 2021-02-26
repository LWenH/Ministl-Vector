#ifndef MINISTL_TYPE_TRAITS_H
#define MINISTL_TYPE_TRAITS_H

namespace ministl
{
    template <class T,T v>
    struct m_integral_constant
    {
        static constexpr T value = v;
    };

    //实现类似enable_if的效果
    template <bool b>
    using m_bool_constant = m_integral_constant<bool ,b>;

    typedef m_bool_constant<true>       m_true_type;
    typedef m_bool_constant<false>      m_false_type;

    //前向声明
    template <class T,class U>
    struct pair;

    template <class T>
    struct is_pair : public m_false_type {};

    template <class T1,class T2>
    struct is_pair<pair<T1,T2>> : public m_false_type{};

}

#endif //MINISTL_TYPE_TRAITS_H
