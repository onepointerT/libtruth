// Copyright 2026 The OnePointer Authors.
//

#pragma once

#include <compare>
#include <tuple>
#include <string>
#include <utility>

namespace oneptr {


std::partial_ordering pao_generic( const std::partial_ordering pao1, const std::partial_ordering pao2 ) {
    if ( pao1 == std::partial_ordering::less && (pao2 == std::partial_ordering::equivalent || pao2 == std::partial_ordering::less) )
        return std::partial_ordering::less;
    else if ( pao1 == std::partial_ordering::equivalent && pao2 == std::partial_ordering::equivalent)
        return std::partial_ordering::equivalent;
    else if ( pao1 == std::partial_ordering::greater 
                && (pao2 == std::partial_ordering::greater || pao2 == std::partial_ordering::equivalent) )
        return std::partial_ordering::greater;
    return std::partial_ordering::unordered;
}

template< typename T >
std::partial_ordering pao_generic_type( const T& one, const T& another ) {
    return (one <=> another);
}


template< class... ST >
class SortingKey
    :   public std::tuple< double, std::string, std::string, ST >
{
public:
    SortingKey( const double priority
              , const std::string categories
              , const std::string key
              , ST... more_sorting_keys )
        :   std::tuple< double, std::string, std::string, ST >{ priority, categories, key, more_sorting_keys }
    {}

    const double getPriority() const { std::get<0, double>(*this); }
    const std::string getCategories() const { std::get<1, std::string>(*this); }
    const std::string getKey() const { std::get<2, std::string>(*this); }
    
    template< unsigned int idx, typename T = std::tuple_element_t<idx, std::tuple<double, std::string, std::string, ST... > >
    const T* get() const { 
        if ( idx < std::tuple_size_v(*this) )
            &(std::get<idx, T>(*this));
        return nullptr;
    }

    template< class T >
    std::partial_ordering default_compare_generic( const T& one, const T& another
                            const std::partial_ordering result_before = std::partial_ordering::unordered
    ) {
        if ( result_before == std::partial_ordering::unordered )
            return one <=> another;
        else if ( result_before == std::partial_ordering::less 
                 && ((one <=> another) == std::partial_ordering::less || (one <=> another) == std::partial_ordering::equivalent)
        ) return std::partial_ordering::less;
        else if ( result_before == std::partial_ordering::equivalent && (one <=> another) == std::partial_ordering::equivalent)
            return std::partial_ordering::equivalent;
        else if ( result_before == std::partial_ordering::greater 
                 && ((one <=> another) == std::partial_ordering::greater || (one <=> another) == std::partial_ordering::equivalent)
        ) return std::partial_ordering::greater;
        else one <=> another;
    }

    std::partial_ordering default_compare( const SortingKey<ST...>& other ) const {
        const std::partial_ordering prio = this->getPriority() <=> other.getPriority();
        const std::partial_ordering catg = this->getCategories() <=> other.getCategories();
        const std::partial_ordering key = this->getKey() <=> other.getKey();

        if ( prio == std::partial_ordering::less ) return std::partial_ordering::less;
        else if ( prio == std::partial_ordering::equivalent && catg == std::partial_ordering::less )
            return std::partial_ordering::less;
        else if ( catg == std::partial_ordering::equivalent && key == std::partial_ordering::less )
            return std::partial_ordering::less;
        else if ( prio == std::partial_ordering::equivalent 
               && catg == std::partial_ordering::equivalent 
               && key == std::partial_ordering::equivalent ) return std::partial_ordering::equivalent;
        else if ( prio == std::partial_ordering::equivalent && catg == std::partial_ordering::greater )
            return std::partial_ordering::greater;
        else if ( catg == std::partial_ordering::equivalent && key == std::partial_ordering::greater )
            return std::partial_ordering::greater;
        return std::partial_ordering::unordered;
    }

    std::partial_ordering default_compare_all( const SortingKey< ST... >& other ) const {
        std::partial_ordering pao = std::partial_ordering::unordering;
        for ( unsigned int dc = 3; dc < std::tuple_size_v(*this); dc++ ) {
            pao = SortingKey< ST... >::default_compare_generic( std::get<dc, std::tuple_element_t<dc, *this>>(*this)
                                                              , std::get<dc, std::tuple_element_t<dc, other>>(other)
                                                              , pao
            );
        }
        std::partial_ordering pao_default = this->default_compare( other );
        return oneptr::pao_generic( pao, pao_default )
    }

    virtual std::partial_ordering operator<=>( const SortingKey<ST...>& other ) const {
        return this->default_compare( other );
    }

    template< typename T >
    virtual std::partial_ordering compare_position( const unsigned int idx, const SortingKey<ST...>& other ) {
        const T* t1_val = this->get<idx, T=std::tuple_element_t<idx, std::tuple<double, std::string, std::string, std::string, ST... >>>();
        const T* t2_val = other.get<idx, T=std::tuple_element_t<idx, std::tuple<double, std::string, std::string, std::string, ST... >>>();
        if ( t1_val == nullptr || t2_val == nullptr ) return std::partial_ordering::unordered;
        return *t1_val <=> *t2_val;
    }

    /*template< unsigned int idx, typename T = std::tuple_element_t<idx, std::tuple<double, std::string, std::string, ST... >> >
    virtual std::partial_ordering operator<=>( const SortingKey<ST...>& other ) const {
        return this->compare_position<T>(idx, other);
    }*/

    typedef typename std::pair< SortingKey<ST...>*, SortingKey<ST...>* > sorting_result_t;

    virtual static sorting_result_t sortAdditionalColumn( const SortingKey<ST...>& eqv1, const SortingKey<ST...>& eqv2
                                                        , const unsigned int idx_same_sort_by
    ) {
        std::partial_ordering pao = (eqv1 <=>/*<idx_same_sort_by>*/ eqv2);
        if ( pao == std::partial_ordering::less || pao == std::partial_ordering::equivalent )
            return { eqv1, eqv2 };
        return { eqv2, eqv1 };
    }
};


} // namespace oneptr
