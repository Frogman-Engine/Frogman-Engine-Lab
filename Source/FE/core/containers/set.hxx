#ifndef _FE_CORE_SET_HXX_
#define _FE_CORE_SET_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <set>
#include <memory_resource>




BEGIN_NAMESPACE(FE)


template<class Key, size_t Capacity, class Comparator = std::less<Key>>
class fset : public std::pmr::set<Key, Comparator>
{
public:
	using base_type = std::pmr::set<Key, Comparator>;
    using key_type = Key;
    using key_compare = Comparator;
    using value_compare = typename base_type::value_compare;
    using value_type = typename base_type::value_type;
    using allocator_type = typename base_type::allocator_type;
    using size_type = typename base_type::size_type;
    using difference_type = typename base_type::difference_type;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;

private:
    std::array<std::byte, sizeof(Key)* Capacity> m_actual_memory;
    std::pmr::monotonic_buffer_resource m_memory_resource;

public:
	_FORCE_INLINE_ fset() noexcept : m_actual_memory(), m_memory_resource(m_actual_memory.data(), m_actual_memory.size()) 
    {
        base_type(std::pmr::polymorphic_allocator<Key>{&m_memory_resource}); 
    }
	_FORCE_INLINE_ fset(const fset& other_p) noexcept : m_actual_memory(), m_memory_resource(m_actual_memory.data(), m_actual_memory.size()) 
    {
        base_type(std::pmr::polymorphic_allocator<Key>{&m_memory_resource});
        const base_type* l_other = &other_p;
        base_type* l_this = this;

        *l_this = *l_other;
    }
	_FORCE_INLINE_ fset(fset&& rvalue_p) noexcept : m_actual_memory(), m_memory_resource(m_actual_memory.data(), m_actual_memory.size()) 
    {
        base_type(std::pmr::polymorphic_allocator<Key>{&m_memory_resource});
        base_type* l_rvalue = &rvalue_p;
        base_type* l_this = this;

        *l_this = std::move(*l_rvalue);
    }

	_FORCE_INLINE_ boolean is_empty() const noexcept
	{
		return base_type::empty();
	}

};


END_NAMESPACE
#endif