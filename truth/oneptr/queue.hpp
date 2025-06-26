// Copyright 2025 The OnePointer Authors.
//

#pragma once


#include <deque>


namespace oneptr {


template< typename T >
class queue_element {
protected:
    T* m_element;

public:
    /// @brief A pointer to the previous queue element
    queue_element< T >* prev;
    /// @brief A pointer to the next queue element
    queue_element< T >* next;

    queue_element( T* value, queue_element< T >* value_next = nullptr, queue_element< T >* value_previous = nullptr )
        :   m_element( value )
        ,   prev( value_previous )
        ,   next( value_next )
    {}
    queue_element( T& value, queue_element< T >* value_next = nullptr, queue_element< T >* value_previous = nullptr  )
        :   m_element( &value )
        ,   prev( value_previous )
        ,   next( value_next )
    {}
    queue_element( queue_element< T >& another )
        :   m_element( another.get() )
        ,   prev( another.prev )
        ,   next( another.next )
    {}

    void clear() {
        this->prev = nullptr;
        this->next = nullptr;
    }
    void reset() {
        this->clear();
        this->m_element = nullptr;
    }

    T* operator()() {
        return this->m_element;
    }
    const T* operator()() const {
        return this->m_element;
    }
    T* get() {
        return this->m_element;
    }
    const T* get() const {
        return this->m_element;
    }
};


/// @brief A enhanced queue, over `std::deque< T >` it saves in queue_elements in a doubly-pointer-and-linked behaviour.
/// @tparam T The type of objects to save inside of the `deque`.
// TODO: Enhance with own queue_element type template parameter
template< typename T >
class queue
    :   public std::deque< oneptr::queue_element< T >* >
{
public:
    typedef std::deque< oneptr::queue_element< T >* >::iterator iterator;
    typedef std::deque< oneptr::queue_element< T >* >::const_iterator const_iterator;
    typedef std::deque< oneptr::queue_element< T >* >::size_type size_type;

    typedef typename oneptr::queue_element< T >* element_t;

    queue()
        :   std::deque< oneptr::queue_element< T >* >()
    {}
    queue( const std::deque< T >&& other)
        :   std::deque< oneptr::queue_element< T >* >( std::ranges::subrange{ other.begin(), other.end() } )
    {}
    queue( const std::initializer_list<T> ils )
        :   std::deque< oneptr::queue_element< T >* >( std::ranges::subrange{ ils.begin(), ils.end() } )
    {}

    template< class... Args >
    void emplace_front( Args&&... args ) {
        queue_element< T >* nxt = this->front();
        queue_element< T >* prv = nullptr;
        for ( auto arg : std::vector{ args... } ) {
            queue_element< T >* qe = new queue_element< T >( arg );
            qe->next = nxt;
            qe->prev = prv;
            nxt->prev = qe;
            nxt = qe;

            std::deque< oneptr::queue_element< T >* >::emplace_front( qe );
        }
    }

    template< class... Args >
    void emplace_back( Args&&... args ) {
        queue_element< T >* nxt = nullptr;
        queue_element< T >* prv = this->back();
        for ( auto arg : std::vector{ args... } ) {
            queue_element< T >* qe = new queue_element< T >( arg );
            qe->next = nxt;
            qe->prev = prv;
            prv->next = qe;
            prv = qe;

            std::deque< oneptr::queue_element< T >* >::emplace_back( qe );
        }
    }

    template< class... Args >
    void emplace( const_iterator pos, Args&&... args ) {
        queue_element< T >* nxt = *pos;
        queue_element< T >* prv = *pos; --prv;
        queue_element< T >* qe = nullptr;
        for ( T&& arg : { args... } ) {
            qe = new queue_element< T >( arg );
            qe->next = nxt;
            qe->prev = prv;
            nxt->prev = qe;
            prv->next = qe;
            prv = qe;

            std::deque< oneptr::queue_element< T >* >::emplace( pos, qe );
        }
        ++nxt; nxt->prev = qe;
    }

    iterator erase( const_iterator pos ) {
        iterator it = pos;
        it->next = it->prev;
        it->prev = it->next;
        return std::deque< oneptr::queue_element< T >* >::erase( pos );
    }

    iterator erase( const_iterator first, const_iterator last ) {
        (first-1)->next = (last+1);
        (last+1)->prev = (first-1);
        return std::deque< oneptr::queue_element< T >* >::erase( first, last );
    }
    
    iterator insert( const_iterator pos, const T& value ) {
        queue_element< T >* qe = new queue_element< T >( &value );
        qe->next = *pos;
        qe->prev = (*pos)->next;
        qe->prev->next = qe;
        iterator it = std::deque< queue_element< T >* >::insert( pos, qe );
        return it;
    }

    iterator insert( const_iterator pos, T&& value ) {
        queue_element< T >* qe = new queue_element< T >( value );
        const_iterator prv = pos; --prv;
        qe->next = *pos;
        qe->prev = prv;
        qe->prev->next = qe;
        iterator it = std::deque< queue_element< T >* >::insert( pos, qe );
        pos->prev = qe;
        return it;
    }

    iterator insert( const_iterator pos,
                    size_type count, const T& value ) {
        iterator it = pos;
        for ( unsigned int c = 0; c < count; c++ ) {
            queue_element< T >* qe = new queue_element< T >( value );
            const_iterator prv = pos; --prv;
            qe->next = *pos;
            qe->prev = prv;
            qe->prev->next = qe;
            it = std::deque< queue_element< T >* >::insert( pos, qe );
            qe->next->prev = qe;
        }
        return it;
    }

    template< class InputIt >
    iterator insert( const_iterator pos, InputIt first, InputIt last ) {
        
        return std::deque< queue_element< T >* >::insert_range( pos, std::ranges::subrange{ first, last } );
    }

    iterator insert( const_iterator pos, std::initializer_list<T> ilist ) {
        iterator it;
        for ( T value : ilist ) {
            it = this->insert( pos, value );
        }
        return it;
    }

    iterator insert( const_iterator pos, std::initializer_list<T*> ilist ) {
        iterator it;
        for ( T* value : ilist ) {
            queue_element< T >* qe = new queue_element< T >( value, (*pos)->prev, *pos );
            it = std::deque< oneptr::queue_element< T >* >::insert( pos, qe );
            qe->next->prev = qe;
            qe->prev->next = qe;
        }
        return it;
    }

    template<class _Rng>
    iterator insert_range( const_iterator pos, _Rng&& r ) {
        iterator it = this->end();
        for ( auto elem : r ) {
            it = this->insert( pos, {elem} );
        }
        return it;
    }

    void push_back( const T& value ) {
        queue_element< T >* qe = new queue_element< T >( &value, nullptr, this->back() );
        std::deque< oneptr::queue_element< T >* >::push_back( qe );
    }

    void push_back( T&& value ) {
        queue_element< T >* qe = new queue_element< T >( value, nullptr, this->back() );
        std::deque< oneptr::queue_element< T >* >::push_back( qe );
    }

    void push_back( T* value ) {
        queue_element< T >* qe = new queue_element< T >( value, nullptr, this->back() );
        std::deque< oneptr::queue_element< T >* >::push_back( qe );
    }

    void push_front( const T& value ) {
        queue_element< T >* qe = new queue_element< T >( value, this->front() );
        std::deque< oneptr::queue_element< T >* >::push_front( qe );
    }

    void push_front( T&& value ) {
        queue_element< T >* qe = new queue_element< T >( value, this->front() );
        std::deque< oneptr::queue_element< T >* >::push_front( qe );
    }

    void push_front( T* value ) {
        queue_element< T >* qe = new queue_element< T >( value, this->front() );
        std::deque< oneptr::queue_element< T >* >::push_front( qe );
    }
};


} // namespace oneptr

