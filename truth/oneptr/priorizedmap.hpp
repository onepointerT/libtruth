// Copyright 2026 The OnePointer Authors.
//

#pragma once

#include "priorizedsortingkey.hpp"

#include <compare>
#include <initializer_list>
#include <list>
#include <utility>


namespace oneptr {


template< class Type, class... SortingKeyT/*, class SKT = SortingKey< SortingKeyT... >*/ >
class PriorizedMapElement
    :   public Type
    ,   public SortingKey< SortingKeyT... >
{
public:
    PriorizedMapElement( Type& element, const double priorizement
                       , const std::string categories, const std::string key
                       , SortingKeyT... more_sorting_keys
    );

    virtual std::partial_ordering operator<=>( const typename PriorizedMapElement< Type, SortingKeyT... >& other ) {
        return this->default_compare_all( other );
    }

    /*template< unsigned int idx >
    virtual std::partial_ordering operator<=>( const typename PriorizedMapElement< Type, SortingKeyT... >& other ) {
        return this->compare_position<idx>( other );
    }*/

    typedef typename std::pair< typename PriorizedMapElement<Type, SortingKeyT...>*
                              , typename PriorizedMapElement<Type, SortingKeyT...>* > sorted_result_t;

    virtual static const sorted_result_t getSorted( typename PriorizedMapElement<Type, SortingKeyT...>& one
                                                  , typename PriorizedMapElement<Type, SortingKeyT...>& another
    ) {
        const std::partial_ordering pao = (one <=> another);
        if ( pao == std::partial_ordering::less || pao == std::partial_ordering::equivalent ) return { one, another };
        else return { another, one };
    }

    virtual static const sorted_result_t getSortedByKeyIndex( const unsigned int idx_key
                                                            , typename PriorizedMapElement<Type, SortingKeyT...>& one
                                                            , typename PriorizedMapElement<Type, SortingKeyT...>& another
    ) {
        const std::partial_ordering pao = one <=>/*<idx_key>*/ another;
        if ( pao == std::partial_ordering::less || pao == std::partial_ordering::equivalent ) return { one, another };
        else return { another, one };
    }
};



template< class Type, class... SortingKeyT >
class PriorizedMap
    :   protected std::list< PriorizedMapElement< Type, SortingKeyT... >* >
{
public:
    PriorizedMap();

    typedef typename PriorizedMapElement< Type, SortingKeyT... > element_t;
    typedef typename std::list< element_t* > list_t;
    using typename list_t::iterator;
    using typename list_t::const_iterator;
    using list_t::begin;
    using list_t::end;
    using list_t::size;
    using list_t::at;

    typedef typename std::tuple< double, std::string, std::string > priorized_key_t;
    typedef typename std::pair< iterator, iterator > sorted_iterator_t;

    virtual static sorted_iterator_t sort_positions( iterator first, iterator second ) {
        const typename element_t::sorted_result_t srt = element_t::getSorted( *first, *second );
        if ( srt.first == *first ) return { first, second };
        else return { second, first };
    }

    virtual static sorted_iterator_t sort_positions_by_key_index( const unsigned int idx_key
                                                                , iterator first, iterator second
    ) {
        const typename element_t::sorted_result_t srt = element_t::getSortedByKeyIndex( *first, *second );
        if ( srt.first == *first ) return { first, second };
        else return { second, first };
    }

    void swap( iterator first, iterator second ) {
        this->insert( first, *second );
        this->insert( second, *first );
        this->erase( first );
        this->erase( second );
    }

    virtual void sort() {
        iterator first = this->begin();
        while ( first != this->end() ) {
            iterator following = first + 1;
            while ( following != this->end() ) {
                typename sorted_iterator_t sit = this->sort_positions( first, following );
                if ( sit.first != first ) {
                    this->swap( first, following );
                }
                ++following;
            }
            ++first;
        }
    }

    static typename list_t& sortKeyColumn( const unsigned int idx, const typename list_t& l ) {
        typename list_t& lt = *new list_t(l);
        
        iterator first = lt.begin();
        while ( first != lt.end() ) {
            iterator following = first + 1;
            while ( following != lt.end() ) {
                
                
                typename sorted_iterator_t sit = this->sort_positions_by_key_index( idx, first, following );
                if ( sit.first != first ) {
                    lt.insert( first, *following );
                    lt.insert( following, *first );
                    lt.erase( following );
                    lt.erase( first );
                }
                ++following;
            }
            ++first;
        }

        return lt;
    }

    static typename list_t* sortPreOrderedList( const typename list_t& lordered
                                              , const unsigned int additional_column_idx
    ) {
        typename list_t& l = *new list_t(lordered);

        for ( typename iterator it = l.begin(); it != l.end(); ++it ) {
            if ( (*it <=> *(it+1)) == std::partial_ordering::equivalent ) {
                typename PriorizedMapElement< Type, SortingKeyT... >::sorting_result_t srt
                        = PriorizedMapElement< Type, SortingKeyT... >::sortAdditionalColumn( *it, *(it+1), additional_column_idx );
                if ( *it != srt.first ) {
                    l.insert(it, *(it+1));
                    l.erase((it+1));
                }
            } else continue;
        }

        return &l;
    }

    virtual typename list_t& sortBy( const std::initializer_list<unsigned int> sorting_key_idxs ) {
        typename list_t& current_result = *new list_t(*this);
        current_result = this->sortKeyColumn( sorting_key_idxs[0], current_result );

        for ( unsigned int sk_idx : sorting_key_idxs ) {
            if ( sk_idx == sorting_key_idxs[0] ) continue;
            typename list_t* ltmp = this->sortPreOrderedList( current_result, sk_idx );
            if ( ltmp == nullptr ) break;
            else current_result = *ltmp;
        }

        return current_result;
    }

    virtual void sortByPriority( const bool honor_category = false ) {
        if ( honor_category )
            this->swap( this->sortBy( {0, 1, 2} ) );
        else this->swap( this->sortBy( {0, 2} ) );
    }

    virtual void sortByCategory( const bool honor_key = true ) {
        if ( honor_key )
            this->swap( this->sortBy( { 1, 0, 2 } ) );
        else this->swap( this->sortBy( { 1, 0 } ) );
    }

    virtual void sortByPriorizedAnything( std::initializer_list<unsigned int> anything_sortOrdering ) {
        this->swap( this->sortBy( {0} + anything_sortOrdering ) );
    }
};


} // namespace oneptr
