#ifndef _FE_CORE_POOL_HXX_
#define _FE_CORE_POOL_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/containers/stack.hxx>
#include <FE/core/containers/queue.hxx>
#include <FE/core/hash.hpp>
#include <array>
#include <memory>
#include <unordered_map>
#pragma warning (push)
#pragma warning (disable: 6262)




BEGIN_NAMESPACE(FE)


enum struct POOL_TYPE : uint8
{
    _BLOCK = 0,
    _GENERIC = 1,
};

struct memory_region_t 
{
    const char* _name = " ";
};


namespace internal::pool
{
    template <typename T, POOL_TYPE PoolType>
    struct block_info;


    template <typename T>
    struct block_info<T, POOL_TYPE::_BLOCK>
    {
        using value_type = T;
        using pointer = value_type*;

        pointer _address = nullptr;
    };

    template <>
    struct block_info<void, POOL_TYPE::_GENERIC>
    {
        std::byte* _address = nullptr;
        var::size_t _size_in_bytes = 0;
    };


    struct from_low_address final
    {
        _NODISCARD _FORCE_INLINE_ boolean operator()(const block_info<void, POOL_TYPE::_GENERIC>& left_p, const block_info<void, POOL_TYPE::_GENERIC>& right_p) const noexcept
        {
            return (left_p._address > right_p._address);
        }
    };


    template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment>
    struct chunk
    {
        FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
        FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

        using block_info_type = block_info<T, POOL_TYPE::_BLOCK>;
        using value_type = typename block_info_type::value_type;
        using pointer = typename block_info_type::pointer;

    private:
        alignas(FE::SIMD_auto_alignment::alignment_type::size) std::array<std::byte, sizeof(T)* ChunkCapacity> m_memory;

    public:
        constexpr static size_t chunk_capacity = ChunkCapacity;

        FE::fstack<block_info_type, ChunkCapacity> _unused_blocks;
        pointer const _begin = reinterpret_cast<pointer const>(m_memory.data());
        pointer _page_iterator = _begin;
        pointer const _end = _begin + ChunkCapacity;

        _FORCE_INLINE_ boolean is_full() const noexcept
        {
            return (_unused_blocks.is_empty() == true) && (_page_iterator >= _end);
        }
    };

    template<size_t ChunkCapacity, class Alignment>
    struct chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>
    {
        using block_info_type = block_info<void, POOL_TYPE::_GENERIC>;

    private:
        alignas(FE::SIMD_auto_alignment::alignment_type::size) std::array<std::byte, ChunkCapacity> m_memory;

    public:
        constexpr static count_t chunk_capacity = ChunkCapacity;
        constexpr static size_t recycler_capacity = ChunkCapacity / Alignment::size;

        /*
         std::pair's _address contains the address of the memory block.
         std::pair's _size_in_bytes contains the size of the memory block.
        */
        FE::fpriority_queue<block_info_type, recycler_capacity, from_low_address> _unused_blocks;
        std::byte* const _begin = m_memory.data();
        std::byte* _page_iterator = _begin;
        std::byte* const _end = _begin + m_memory.size();

        _FORCE_INLINE_ boolean is_full() const noexcept
        {
            return (_unused_blocks.is_empty() == true) && (_page_iterator >= _end);
        }
    };
}


template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
struct pool_deleter;

template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
class pool;


template<typename T, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
struct pool_deleter<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, StatefulAllocator>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

    using chunk_type = internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment>;
    using value_type = typename FE::remove_const_reference<typename chunk_type::value_type>::type;
    using block_info_type = typename chunk_type::block_info_type;

private:
    chunk_type* m_host_chunk = nullptr;

public:
    constexpr static count_t chunk_capacity = ChunkCapacity;

    _FORCE_INLINE_ pool_deleter() noexcept : m_host_chunk() {}
    _FORCE_INLINE_ pool_deleter(chunk_type* host_p) noexcept : m_host_chunk(host_p) {}

    _FORCE_INLINE_ void operator()(value_type* ptr_p) const noexcept
    {
        FE_ASSERT(this->m_host_chunk == nullptr, "${%s@0}: ${%s@1} was nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_host_chunk));
        if (ptr_p == nullptr || this->m_host_chunk == nullptr) { return; }

        if constexpr (FE::is_trivial<value_type>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
        {
            ptr_p->~value_type();
        }
   
        this->m_host_chunk->_unused_blocks.push(block_info_type{ ptr_p });
    }
};


template<size_t ChunkCapacity, class Alignment>
struct generic_deleter_base
{
    template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
    friend class pool;

    using chunk_type = internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>;
    using block_info_type = internal::pool::block_info<void, POOL_TYPE::_GENERIC>;

    constexpr static size_t chunk_capacity = ChunkCapacity;
    constexpr static size_t temporary_storage_capacity = ChunkCapacity / Alignment::size;

protected:
    chunk_type* m_host_chunk = nullptr;
    var::size_t m_size_in_bytes = 0;
    thread_local static FE::fstack<block_info_type, temporary_storage_capacity> tl_s_temporary_storage;

public:
    _FORCE_INLINE_ generic_deleter_base() noexcept : m_host_chunk(), m_size_in_bytes() {}
    _FORCE_INLINE_ generic_deleter_base(chunk_type* host_p, size_t size_in_bytes_p) noexcept : m_host_chunk(host_p), m_size_in_bytes(size_in_bytes_p) {}

    _FORCE_INLINE_ chunk_type* get_host_chunk_ptr() const noexcept
    {
		return this->m_host_chunk;
	}

    _FORCE_INLINE_ size_t get_size_in_bytes() const noexcept
    {
        return this->m_size_in_bytes;
    }
};

template<size_t ChunkCapacity, class Alignment>
thread_local FE::fstack<typename generic_deleter_base<ChunkCapacity, Alignment>::block_info_type, generic_deleter_base<ChunkCapacity, Alignment>::temporary_storage_capacity> generic_deleter_base<ChunkCapacity, Alignment>::tl_s_temporary_storage;


template<size_t ChunkCapacity, class Alignment, class StatefulAllocator>
struct nondestructive_generic_deleter final : public generic_deleter_base<ChunkCapacity, Alignment>
{
    using base_type = generic_deleter_base<ChunkCapacity, Alignment>;
    using chunk_type = typename base_type::chunk_type;
    using block_info_type = typename base_type::block_info_type;

    _FORCE_INLINE_ nondestructive_generic_deleter() noexcept : base_type() {}
    _FORCE_INLINE_ nondestructive_generic_deleter(chunk_type* host_p, size_t size_in_bytes_p) noexcept : base_type(host_p, size_in_bytes_p) {}

    _FORCE_INLINE_ void operator()(void* ptr_p) const noexcept
    {
        FE_ASSERT(this->m_host_chunk == nullptr, "${%s@0}: ${%s@1} was nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_host_chunk));

        if (ptr_p == nullptr || this->m_host_chunk == nullptr) { return; }


        FE_ASSERT(this->m_size_in_bytes == 0, "Assertion Failed: ${%s@0} cannot be zero.", TO_STRING(this->m_size_in_bytes));
        this->m_host_chunk->_unused_blocks.push(block_info_type{ static_cast<std::byte*>(ptr_p), this->m_size_in_bytes });

        if (this->m_host_chunk->_unused_blocks.size() > 1)
        {
            pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>::__merge(base_type::tl_s_temporary_storage, this->m_host_chunk->_unused_blocks);
        }
    }
};


template<typename T, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
struct pool_deleter<T, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator> final : public generic_deleter_base<ChunkCapacity, Alignment>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

    using base_type = generic_deleter_base<ChunkCapacity, Alignment>;
    using chunk_type = typename base_type::chunk_type;
    using block_info_type = typename base_type::block_info_type;
    using value_type = typename FE::remove_const_reference<T>::type;

private:
    var::count_t m_element_count = 0;

public:
    _FORCE_INLINE_ pool_deleter() noexcept : base_type() {}
    _FORCE_INLINE_ pool_deleter(chunk_type* host_p, count_t elements_p, size_t size_in_bytes_p) noexcept : base_type(host_p, size_in_bytes_p), m_element_count(elements_p) {}

    void operator()(void* ptr_p) const noexcept
    {
        FE_ASSERT(this->m_host_chunk == nullptr, "${%s@0}: ${%s@1} was nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_host_chunk));
        FE_ASSERT(this->m_size_in_bytes < this->m_element_count, "Assertion Failed: Detected arguments with illegal values.");

        if (ptr_p == nullptr || this->m_host_chunk == nullptr) { return; }
     
        if constexpr (FE::is_trivial<value_type>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
        {
            value_type* l_ptr = static_cast<value_type*>(ptr_p);
            void* const l_ptr_end = static_cast<value_type*>(ptr_p) + this->m_element_count;

            while (l_ptr != l_ptr_end)
            {
                l_ptr->~value_type();
                ++l_ptr;
            }
        }

        FE_ASSERT(this->m_size_in_bytes == 0, "Assertion Failed: ${%s@0} cannot be zero.", TO_STRING(this->m_size_in_bytes));
        this->m_host_chunk->_unused_blocks.push(block_info_type{ static_cast<std::byte*>(ptr_p), this->m_size_in_bytes });

        if (this->m_host_chunk->_unused_blocks.size() > 1)
        {
            pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>::__merge(base_type::tl_s_temporary_storage, this->m_host_chunk->_unused_blocks);
        }
    }

    _FORCE_INLINE_ count_t get_element_count() const noexcept
    {
        return this->m_element_count;
    }
};



// + memory pool regions
template<typename T, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
class pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, StatefulAllocator>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

public:
    using chunk_type = internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment>;
    using deleter_type = pool_deleter<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, StatefulAllocator>;
    using block_info_type = typename chunk_type::block_info_type;

    using global_pool_type = std::list<chunk_type, StatefulAllocator>;

    constexpr static count_t chunk_capacity = ChunkCapacity;

    FE_STATIC_ASSERT((std::is_same<T, typename chunk_type::value_type>::value == false), "Static Assertion Failed: The value_type does not match.");
    FE_STATIC_ASSERT((std::is_same<T*, typename chunk_type::pointer>::value == false), "Static Assertion Failed: The value_type* does not match.");

private:
    thread_local static typename global_pool_type tl_s_global_memory;

public:
    static std::unique_ptr<T, deleter_type> allocate() noexcept
    {
        if ((tl_s_global_memory.empty() == true)) _UNLIKELY_
        {
            create_pages(1);
        }

        typename global_pool_type::iterator l_list_iterator = tl_s_global_memory.begin();
        typename global_pool_type::const_iterator l_cend = tl_s_global_memory.cend();

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->is_full() == false)
            {
                T* l_value;
                if (l_list_iterator->_unused_blocks.is_empty() == false)
                {
                    l_value = l_list_iterator->_unused_blocks.pop()._address;
                    
                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }
                }
                else
                {
                    l_value = l_list_iterator->_page_iterator;

                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }
                    
                    ++(l_list_iterator->_page_iterator);
                }

                return std::unique_ptr<T, deleter_type>(l_value, deleter_type(l_list_iterator.operator->()));
            }
            else
            {
                create_pages(1);
                continue;
            }
        }

        create_pages(1);
        return allocate();
    }

    _FORCE_INLINE_ static void create_pages(size_t chunk_count_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_global_memory.emplace_back();
        }
    }

    static void shrink_to_fit() noexcept
    {
        typename global_pool_type::iterator l_list_iterator = tl_s_global_memory.begin();
        typename global_pool_type::const_iterator l_cend = tl_s_global_memory.cend();
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: Unable to shrink_to_fit an empty memory pool");


        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_element_size = l_list_iterator->_unused_blocks.size();

            FE_ASSERT((l_list_iterator->_end - l_list_iterator->_begin) != ChunkCapacity, "The chunk range is invalid.");
          
            if (l_list_iterator->_page_iterator < l_list_iterator->_end)
            {
                l_unused_element_size += (l_list_iterator->_end - l_list_iterator->_page_iterator);
            }

            if (l_unused_element_size == ChunkCapacity)
            {
                tl_s_global_memory.erase(l_list_iterator);

                if (tl_s_global_memory.size() == 0)
                {
                    break;
                }

                l_list_iterator = tl_s_global_memory.begin();
            }
        }
    }
};


template<typename T, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
thread_local typename pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, StatefulAllocator>::global_pool_type pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, StatefulAllocator>::tl_s_global_memory;




template<size_t ChunkCapacity, class Alignment, class StatefulAllocator>
class pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>
{
    template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class StatefulAllocator>
    friend struct pool_deleter;

    template<size_t ChunkCapacity, class Alignment, class StatefulAllocator>
    friend struct nondestructive_generic_deleter;

public:
    using chunk_type = internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>;

    template<typename U>
    using deleter_type = pool_deleter<U, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>;

    using global_pool_type = std::list<chunk_type, StatefulAllocator>;

    using block_info_type = typename chunk_type::block_info_type;

    constexpr static size_t chunk_capacity = chunk_type::chunk_capacity;
    constexpr static size_t recycler_capacity = chunk_type::recycler_capacity;

private:

    thread_local static typename global_pool_type tl_s_global_memory;

public:
    template<typename U>
    static std::unique_ptr<U, deleter_type<U>> allocate(count_t size_p = 1) noexcept
    {
        FE_STATIC_ASSERT((Alignment::size % 2) != 0, "Static Assertion Failed: The Alignment::size must be an even number.");
        FE_STATIC_ASSERT(std::is_array<U>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
        FE_STATIC_ASSERT(std::is_const<U>::value == true, "Static Assertion Failed: The T must not be a const type.");
        FE_ASSERT(size_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(size_p));

        size_t l_queried_allocation_size_in_bytes = FE::calculate_aligned_memory_size_in_bytes<U, Alignment>(size_p);

        if ((tl_s_global_memory.empty() == true)) _UNLIKELY_
        {
            create_pages(1);
        }

        typename global_pool_type::iterator l_list_iterator = tl_s_global_memory.begin();
        typename global_pool_type::const_iterator l_cend = tl_s_global_memory.cend();

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->is_full() == false)
            {
                void* l_allocation_result = nullptr;
                block_info_type l_memblock_info;

                if (l_list_iterator->_unused_blocks.is_empty() == false)
                {
                    /*
                    block_info_type's _address contains the address of the memory block.
                    block_info_type's _size_in_bytes contains the size of the memory block.
                    */

                    __recycle<U>(l_memblock_info, *l_list_iterator, l_queried_allocation_size_in_bytes);

                    if (l_memblock_info._size_in_bytes < l_queried_allocation_size_in_bytes)
                    {
                        if ((l_list_iterator->_page_iterator + l_queried_allocation_size_in_bytes) >= l_list_iterator->_end)
                        {
                            create_pages(1);
                            continue;;
                        }

                        l_allocation_result = l_list_iterator->_page_iterator;
                        l_list_iterator->_page_iterator += l_queried_allocation_size_in_bytes;
                    }
                    else
                    {
                        l_allocation_result = l_memblock_info._address;
                    }
                }
                else
                {
                    if ((l_list_iterator->_page_iterator + l_queried_allocation_size_in_bytes) >= l_list_iterator->_end)
                    {
                        create_pages(1);
                        continue;
                    }

                    l_allocation_result = l_list_iterator->_page_iterator;
                    l_list_iterator->_page_iterator += l_queried_allocation_size_in_bytes;
                }


                if constexpr (FE::is_trivial<U>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                {
                    U* l_iterator = static_cast<U*>(l_allocation_result);
                    for (var::index_t i = 0; i < size_p; ++i)
                    {
                        new(l_iterator) U();
                        ++l_iterator;
                    }
                }


                if (l_list_iterator->_page_iterator > l_list_iterator->_end)
                {
                    create_pages(1);
                    continue;
                }

                return std::unique_ptr<U, deleter_type<U>>{static_cast<U*>(l_allocation_result), deleter_type<U>{reinterpret_cast<chunk_type*>(l_list_iterator.operator->()), size_p, l_queried_allocation_size_in_bytes}};
            }   
            else
            {
                create_pages(1);
            }
        }

        create_pages(1);
        return allocate<U>(size_p);
    }

    _FORCE_INLINE_ static void create_pages(size_t chunk_count_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_global_memory.emplace_back();
        }
    }

    static void shrink_to_fit() noexcept
    {
        typename global_pool_type::iterator  l_list_iterator = tl_s_global_memory.begin();
        typename global_pool_type::const_iterator l_cend = tl_s_global_memory.cend();
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: Unable to shrink_to_fit an empty memory pool");

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_memory_size_in_bytes = 0;
            auto l_container = l_list_iterator->_unused_blocks.get_container();
            for (auto block : l_container)
            {
                l_unused_memory_size_in_bytes += block._size_in_bytes;
            }

            if (l_list_iterator->_page_iterator < l_list_iterator->_end)
            {
                l_unused_memory_size_in_bytes += l_list_iterator->_end - l_list_iterator->_page_iterator;
            }

            if (l_unused_memory_size_in_bytes == ChunkCapacity)
            {
                tl_s_global_memory.erase(l_list_iterator);

                if (tl_s_global_memory.size() == 0)
                {
                    break;
                }

                l_list_iterator = tl_s_global_memory.begin();
            }
        }
    }


    template <typename T>
    static void deallocate(T* pointer_p, count_t element_count_p) noexcept 
    {
        typename global_pool_type::iterator  l_list_iterator = tl_s_global_memory.begin();
        typename global_pool_type::const_iterator l_cend = tl_s_global_memory.cend();
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: Unable to return an address to an empty memory pool.");
        
        std::byte* const l_value = reinterpret_cast<std::byte*>(pointer_p);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if ((l_list_iterator->_begin <= l_value) && (l_value < l_list_iterator->_end))
            {
                if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                {
                    for (var::count_t i = 0; i < element_count_p; ++i)
                    {
                        pointer_p->~T();
                        ++pointer_p;
                    }
                }
                l_list_iterator->_unused_blocks.push(block_info_type{ l_value, FE::calculate_aligned_memory_size_in_bytes<T, Alignment>(element_count_p) });

                if (l_list_iterator->_unused_blocks.size() > 1)
                {
                    __merge(deleter_type<T>::base_type::tl_s_temporary_storage, l_list_iterator->_unused_blocks);
                }
                return;
            }
        }
    }

private:
    static void __merge(FE::fstack<block_info_type, recycler_capacity>& temporary_storage_p, FE::fpriority_queue<block_info_type, recycler_capacity, internal::pool::from_low_address>& unused_scattered_blocks_p) noexcept
    {
        /*
                        block_info_type's _address contains the address of the memory block.
                        block_info_type's _size_in_bytes contains the size of the memory block.
        */

        while (unused_scattered_blocks_p.size() > 1)
        {
            auto l_prev = unused_scattered_blocks_p.top();
            unused_scattered_blocks_p.pop();
            
            auto l_next = unused_scattered_blocks_p.top();

            FE_ASSERT(l_prev._address >= l_next._address, "Assertion Failure: The priority queue has illegal address order. ${%s@0} always has lower address value than ${%s@1}.", TO_STRING(l_prev._address), TO_STRING(l_next._address));
            FE_ASSERT(l_prev._address + l_prev._size_in_bytes > l_next._address, "${%s@0}: Free-ed memory block range collision detected!\nPlease check if the count value passed to generic_pool<>::deallocate<T, Alignment>(T* ptr_p, count_t element_count_p) was incorrect or not.", TO_STRING(FE::MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));

            if ((l_prev._address + l_prev._size_in_bytes) == l_next._address)
            {
                unused_scattered_blocks_p.pop();
                unused_scattered_blocks_p.push({ l_prev._address, l_prev._size_in_bytes + l_next._size_in_bytes });
            }
            else
            {
                temporary_storage_p.push(l_prev);
            }
        }
       
        while(temporary_storage_p.is_empty() == false)
		{
			unused_scattered_blocks_p.push(std::move(temporary_storage_p.pop()));
		}
    }

    template <typename T>
    static void __recycle(block_info_type& in_out_memblock_info_p, chunk_type& memory_p, size_t queried_allocation_size_in_bytes_p) noexcept
    {
        FE_ASSERT(memory_p._unused_blocks.is_empty() == true, "Assertion Failure: Cannot recycle from an empty bin.");

        /*
           block_info_type's _address contains the address of the memory block.
           block_info_type's _size_in_bytes contains the size of the memory block.
           */

        if (memory_p._unused_blocks.size() > 1)
        {
            __merge(deleter_type<T>::base_type::tl_s_temporary_storage, memory_p._unused_blocks);
        }

        in_out_memblock_info_p = memory_p._unused_blocks.top();
        while (in_out_memblock_info_p._size_in_bytes < queried_allocation_size_in_bytes_p)
        {
            deleter_type<T>::base_type::tl_s_temporary_storage.push(std::move(in_out_memblock_info_p));
            memory_p._unused_blocks.pop();

            if (memory_p._unused_blocks.is_empty() == true)
            {
                break;
            }
            else
            {
                in_out_memblock_info_p = memory_p._unused_blocks.top();
            }
        }

        if (memory_p._unused_blocks.is_empty() == false)
        {
            memory_p._unused_blocks.pop();
        }

        int64 l_remaining_size = (in_out_memblock_info_p._size_in_bytes - queried_allocation_size_in_bytes_p);
        if (l_remaining_size > 0)
        {
            memory_p._unused_blocks.push(block_info_type{ (in_out_memblock_info_p._address + queried_allocation_size_in_bytes_p), static_cast<size_t>(l_remaining_size) });
            in_out_memblock_info_p._size_in_bytes = queried_allocation_size_in_bytes_p;
        }

        while (deleter_type<T>::base_type::tl_s_temporary_storage.is_empty() == false)
        {
            memory_p._unused_blocks.push(std::move(deleter_type<T>::base_type::tl_s_temporary_storage.pop()));
        }

        deleter_type<T>::base_type::tl_s_temporary_storage.pop_all();
    }
};


template<size_t ChunkCapacity, class Alignment, class StatefulAllocator>
thread_local typename pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>::global_pool_type pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>::tl_s_global_memory;




template<typename T, size_t ChunkCapacity = 1024, class StatefulAllocator = FE::std_style::scalable_aligned_allocator<internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>>>
using block_pool = pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>, StatefulAllocator>;

template<typename T, size_t ChunkCapacity = 1024, class StatefulAllocator = FE::std_style::scalable_aligned_allocator<internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>>>
using block_pool_ptr = std::unique_ptr<T, pool_deleter<T, FE::POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>, StatefulAllocator>>;


template<size_t ChunkCapacity = 512 MB, class Alignment = FE::align_8bytes, class StatefulAllocator = FE::std_style::scalable_aligned_allocator<internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>>>
using generic_pool = pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>;

template<typename T, size_t ChunkCapacity = 512 MB, class Alignment = FE::align_8bytes, class StatefulAllocator = FE::std_style::scalable_aligned_allocator<internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>>>
using generic_pool_ptr = std::unique_ptr<T, pool_deleter<T, FE::POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, StatefulAllocator>>;




template<uint64 Capacity>
struct capacity final
{
    _MAYBE_UNUSED_ static constexpr inline size_t size = Capacity;
};




END_NAMESPACE
#pragma warning (pop)
#endif