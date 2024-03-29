﻿#ifndef _FE_CORE_POOL_HXX_
#define _FE_CORE_POOL_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/containers/map.hxx>
#include <FE/core/containers/stack.hxx>
#include <FE/core/fstring.hxx>
#include <FE/core/hash.hpp>
#include <array>
#include <cstdlib>
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

using memory_region_t = FE::fstring<64>;


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

        FE::fstack<block_info_type, ChunkCapacity> _free_blocks;
        pointer const _begin = reinterpret_cast<pointer const>(m_memory.data());
        pointer _page_iterator = _begin;
        pointer const _end = _begin + ChunkCapacity;

        _FORCE_INLINE_ boolean is_full() const noexcept
        {
            return (_free_blocks.is_empty() == true) && (_page_iterator >= _end);
        }
    };

    template<size_t ChunkCapacity, class Alignment>
    struct chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>
    {
    public:
        constexpr static count_t chunk_capacity = ChunkCapacity;
        constexpr static size_t recycler_capacity = ChunkCapacity / Alignment::size;

        //using block_info_type = block_info<void, POOL_TYPE::_GENERIC>;
        using block_info_type = typename FE::fmap<std::byte*, var::size_t, recycler_capacity, from_low_address>::value_type;
        using recycler_type = FE::fmap<std::byte*, var::size_t, recycler_capacity>;
        using recycler_iterator = typename recycler_type::iterator;

    private:
        alignas(FE::SIMD_auto_alignment::alignment_type::size) std::array<std::byte, ChunkCapacity> m_memory;
        /*
         std::pair's first contains the address of the memory block.
         std::pair's second contains the size of the memory block.
        */

    public:
        recycler_type _free_blocks;
        std::byte* const _begin = m_memory.data();
        std::byte* _page_iterator = _begin;
        std::byte* const _end = _begin + m_memory.size();

        _FORCE_INLINE_ boolean is_full() const noexcept
        {
            return (_free_blocks.is_empty() == true) && (_page_iterator >= _end);
        }
    };
}


template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
struct pool_deleter;

template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
class pool;


template<typename T, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
struct pool_deleter<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>
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
   
        this->m_host_chunk->_free_blocks.push(block_info_type{ ptr_p });
    }
};


template<size_t ChunkCapacity, class Alignment>
struct generic_deleter_base
{
    template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
    friend class pool;

    using chunk_type = internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>;
    using block_info_type = typename chunk_type::block_info_type;

    constexpr static size_t chunk_capacity = ChunkCapacity;
    constexpr static size_t temporary_storage_capacity = ChunkCapacity / Alignment::size;

protected:
    chunk_type* m_host_chunk = nullptr;
    var::size_t m_size_in_bytes = 0;
    //thread_local static FE::fstack<block_info_type, temporary_storage_capacity> tl_s_temporary_storage;

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

//template<size_t ChunkCapacity, class Alignment>
//thread_local FE::fstack<typename generic_deleter_base<ChunkCapacity, Alignment>::block_info_type, generic_deleter_base<ChunkCapacity, Alignment>::temporary_storage_capacity> generic_deleter_base<ChunkCapacity, Alignment>::tl_s_temporary_storage;


template<size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
struct nondestructive_generic_deleter final : public generic_deleter_base<ChunkCapacity, Alignment>
{
    using base_type = generic_deleter_base<ChunkCapacity, Alignment>;
    using chunk_type = typename base_type::chunk_type;
    using block_info_type = typename base_type::block_info_type;

    _FORCE_INLINE_ nondestructive_generic_deleter() noexcept : base_type() {}
    _FORCE_INLINE_ nondestructive_generic_deleter(chunk_type* host_p, size_t size_in_bytes_p) noexcept : base_type(host_p, size_in_bytes_p) {}

    _FORCE_INLINE_ void operator()(void* ptr_p) const noexcept
    {
        if (ptr_p == nullptr || this->m_host_chunk == nullptr) { return; }

        FE_ASSERT(this->m_size_in_bytes == 0, "Assertion Failed: ${%s@0} cannot be zero.", TO_STRING(this->m_size_in_bytes));
        
        auto l_insertion_result = this->m_host_chunk->_free_blocks.insert(block_info_type{ static_cast<std::byte*>(ptr_p), this->m_size_in_bytes });
        FE_EXIT(l_insertion_result.second == false, FE::MEMORY_ERROR_1XX::_FATAL_ERROR_DOUBLE_FREE, "Double-free detected.");

        if (this->m_host_chunk->_free_blocks.size() > 1)
        {
            pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::__merge(l_insertion_result.first, this->m_host_chunk->_free_blocks);
        }
    }
};


template<typename T, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
struct pool_deleter<T, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator> final : public generic_deleter_base<ChunkCapacity, Alignment>
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

        FE_ASSERT(this->m_size_in_bytes == 0, "FATAL ERROR: ${%s@0} cannot be zero.", TO_STRING(this->m_size_in_bytes));
        
        auto l_insertion_result = this->m_host_chunk->_free_blocks.insert(block_info_type{ static_cast<std::byte*>(ptr_p), this->m_size_in_bytes });
        FE_EXIT(l_insertion_result.second == false, FE::MEMORY_ERROR_1XX::_FATAL_ERROR_DOUBLE_FREE, "Double-free detected.");

        if (this->m_host_chunk->_free_blocks.size() > 1)
        {
            pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::__merge(l_insertion_result.first, this->m_host_chunk->_free_blocks);
        }
    }

    _FORCE_INLINE_ count_t get_element_count() const noexcept
    {
        return this->m_element_count;
    }
};




template<typename T, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
class pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

public:
    using chunk_type = internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment>;
    using deleter_type = pool_deleter<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>;
    using block_info_type = typename chunk_type::block_info_type;

    using global_pool_type = std::list<chunk_type, GlobalAllocator>;
    using namespace_pool_type = std::unordered_multimap<FE::memory_region_t, chunk_type, FE::hash<FE::memory_region_t>, std::equal_to<FE::memory_region_t>, NamespaceAllocator>;

    constexpr static count_t chunk_capacity = ChunkCapacity;

    FE_STATIC_ASSERT((std::is_same<T, typename chunk_type::value_type>::value == false), "Static Assertion Failed: The value_type does not match.");
    FE_STATIC_ASSERT((std::is_same<T*, typename chunk_type::pointer>::value == false), "Static Assertion Failed: The value_type* does not match.");

private:
    thread_local static typename global_pool_type tl_s_global_memory;
    thread_local static typename namespace_pool_type tl_s_memory_regions;

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
                if (l_list_iterator->_free_blocks.is_empty() == false)
                {
                    l_value = l_list_iterator->_free_blocks.pop()._address;
                    
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
        if(l_list_iterator == l_cend) 
        {
            FE_ASSERT(l_list_iterator == l_cend, "Unable to shrink_to_fit() an empty pool.");
            return; 
        }


        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_element_size = l_list_iterator->_free_blocks.size();

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






    static std::unique_ptr<T, deleter_type> allocate(const char* region_name_p) noexcept
    {
        if ((tl_s_memory_regions.empty() == true)) _UNLIKELY_
        {
            create_pages(region_name_p, 1);
        }

        index_t l_bucket_index = tl_s_memory_regions.bucket(region_name_p);
        typename namespace_pool_type::iterator l_list_iterator = tl_s_memory_regions.begin(l_bucket_index);
        typename namespace_pool_type::const_iterator l_cend = tl_s_memory_regions.cend(l_bucket_index);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->second.is_full() == false)
            {
                T* l_value;
                if (l_list_iterator->second._free_blocks.is_empty() == false)
                {
                    l_value = l_list_iterator->second._free_blocks.pop()._address;

                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }
                }
                else
                {
                    l_value = l_list_iterator->second._page_iterator;

                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }

                    ++(l_list_iterator->second._page_iterator);
                }

                return std::unique_ptr<T, deleter_type>(l_value, deleter_type(&(l_list_iterator->second)));
            }
            else
            {
                create_pages(region_name_p, 1);
                continue;
            }
        }

        create_pages(region_name_p, 1);
        return allocate(region_name_p);
    }

    _FORCE_INLINE_ static void create_pages(const char* region_name_p, size_t chunk_count_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        static chunk_type l_s_initial_value;
  
        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_memory_regions.emplace(region_name_p, l_s_initial_value);
        }
    }

    static void shrink_to_fit(const char* region_name_p) noexcept
    {
        index_t l_bucket_index = tl_s_memory_regions.bucket(region_name_p);
        typename namespace_pool_type::iterator l_list_iterator = tl_s_memory_regions.begin(l_bucket_index);
        typename namespace_pool_type::const_iterator l_cend = tl_s_memory_regions.cend(l_bucket_index);
        if (l_list_iterator == l_cend)
        {
            FE_ASSERT(true, "Unable to shrink_to_fit() an empty pool.");
            return;
        }


        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_element_size = l_list_iterator->second._free_blocks.size();

            FE_ASSERT((l_list_iterator->second._end - l_list_iterator->second._begin) != ChunkCapacity, "The chunk range is invalid.");

            if (l_list_iterator->second._page_iterator < l_list_iterator->second._end)
            {
                l_unused_element_size += (l_list_iterator->second._end - l_list_iterator->second._page_iterator);
            }

            if (l_unused_element_size == ChunkCapacity)
            {
                tl_s_memory_regions.erase(l_list_iterator);

                if (tl_s_memory_regions.size() == 0)
                {
                    break;
                }

                l_list_iterator = tl_s_memory_regions.begin(l_bucket_index);
            }
        }
    }
};


template<typename T, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
thread_local typename pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::global_pool_type pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::tl_s_global_memory;

template<typename T, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
thread_local typename pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::namespace_pool_type pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::tl_s_memory_regions;




template<size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
class pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>
{
    template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
    friend struct pool_deleter;

    template<size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
    friend struct nondestructive_generic_deleter;

public:
    using chunk_type = internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>;
    using recycler_type = typename chunk_type::recycler_type;
    using recycler_iterator = typename chunk_type::recycler_iterator;

    template<typename U>
    using deleter_type = pool_deleter<U, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>;

    using global_pool_type = std::list<chunk_type, GlobalAllocator>;
    using namespace_pool_type = std::unordered_multimap<FE::memory_region_t, chunk_type, FE::hash<FE::memory_region_t>, std::equal_to<FE::memory_region_t>, NamespaceAllocator>;

    using block_info_type = typename chunk_type::block_info_type;

    constexpr static size_t chunk_capacity = chunk_type::chunk_capacity;
    constexpr static size_t recycler_capacity = chunk_type::recycler_capacity;

private:

    thread_local static typename global_pool_type tl_s_global_memory;
    thread_local static typename namespace_pool_type tl_s_memory_regions;

public:
    template<typename U>
    static std::unique_ptr<U, deleter_type<U>> allocate(count_t size_p = 1) noexcept
    {
        FE_STATIC_ASSERT((Alignment::size % 2) != 0, "Static Assertion Failed: The Alignment::size must be an even number.");
        FE_STATIC_ASSERT(std::is_array<U>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
        FE_STATIC_ASSERT(std::is_const<U>::value == true, "Static Assertion Failed: The T must not be a const type.");
        FE_ASSERT(size_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(size_p));
        FE_EXIT(size_p > ChunkCapacity, MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY, "Fatal Error: Unable to allocate the size of memmory that exceeds the pool chunk's capacity.");


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
                internal::pool::block_info<void, POOL_TYPE::_GENERIC> l_memblock_info;

                if (l_list_iterator->_free_blocks.is_empty() == false)
                {
                    /*
                    first contains the address of the memory block.
                    second contains the size of the memory block.
                    */

                    __recycle<U>(l_memblock_info, *l_list_iterator, l_queried_allocation_size_in_bytes);

                    if (l_memblock_info._size_in_bytes < l_queried_allocation_size_in_bytes)
                    {
                        if ((l_list_iterator->_page_iterator + l_queried_allocation_size_in_bytes) >= l_list_iterator->_end)
                        {
                            create_pages(1);
                            continue;
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
        FE_ASSERT(l_list_iterator == l_cend, "Unable to shrink_to_fit() an empty pool.");
        
        /*
                  first contains the address of the memory block.
                  second contains the size of the memory block.
        */

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_memory_size_in_bytes = 0;
            for (auto block : l_list_iterator->_free_blocks)
            {
                l_unused_memory_size_in_bytes += block.second;
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
        FE_ASSERT(l_list_iterator == l_cend, "Unable to request deallocate() to an empty pool.");


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

                auto l_insertion_result = l_list_iterator->_free_blocks.insert(block_info_type{ l_value, FE::calculate_aligned_memory_size_in_bytes<T, Alignment>(element_count_p) });
                FE_EXIT(l_insertion_result.second == false, FE::MEMORY_ERROR_1XX::_FATAL_ERROR_DOUBLE_FREE, "Double-free detected.");
                
                if (l_list_iterator->_free_blocks.size() > 1)
                {
                    __merge(l_insertion_result.first, l_list_iterator->_free_blocks);
                }
                return;
            }
        }
    }






    template<typename U>
    static std::unique_ptr<U, deleter_type<U>> allocate(const char* region_name_p, count_t size_p = 1) noexcept
    {
        FE_STATIC_ASSERT((Alignment::size % 2) != 0, "Static Assertion Failed: The Alignment::size must be an even number.");
        FE_STATIC_ASSERT(std::is_array<U>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
        FE_STATIC_ASSERT(std::is_const<U>::value == true, "Static Assertion Failed: The T must not be a const type.");
        FE_ASSERT(size_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(size_p));
        FE_EXIT(size_p > ChunkCapacity, MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY, "Fatal Error: Unable to allocate the size of memmory that exceeds the pool chunk's capacity.");


        size_t l_queried_allocation_size_in_bytes = FE::calculate_aligned_memory_size_in_bytes<U, Alignment>(size_p);

        if ((tl_s_memory_regions.empty() == true)) _UNLIKELY_
        {
            create_pages(region_name_p, 1);
        }

        index_t l_bucket_index = tl_s_memory_regions.bucket(region_name_p);
        typename namespace_pool_type::iterator l_list_iterator = tl_s_memory_regions.begin(l_bucket_index);
        typename namespace_pool_type::const_iterator l_cend = tl_s_memory_regions.cend(l_bucket_index);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->second.is_full() == false)
            {
                void* l_allocation_result = nullptr;
                internal::pool::block_info<void, POOL_TYPE::_GENERIC> l_memblock_info;

                if (l_list_iterator->second._free_blocks.is_empty() == false)
                {
                    /*
                    first contains the address of the memory block.
                    second contains the size of the memory block.
                    */

                    __recycle<U>(l_memblock_info, l_list_iterator->second, l_queried_allocation_size_in_bytes);

                    if (l_memblock_info._size_in_bytes < l_queried_allocation_size_in_bytes)
                    {
                        if ((l_list_iterator->second._page_iterator + l_queried_allocation_size_in_bytes) >= l_list_iterator->second._end)
                        {
                            create_pages(region_name_p, 1);
                            continue;
                        }

                        l_allocation_result = l_list_iterator->second._page_iterator;
                        l_list_iterator->second._page_iterator += l_queried_allocation_size_in_bytes;
                    }
                    else
                    {
                        l_allocation_result = l_memblock_info._address;
                    }
                }
                else
                {
                    if ((l_list_iterator->second._page_iterator + l_queried_allocation_size_in_bytes) >= l_list_iterator->second._end)
                    {
                        create_pages(region_name_p, 1);
                        continue;
                    }

                    l_allocation_result = l_list_iterator->second._page_iterator;
                    l_list_iterator->second._page_iterator += l_queried_allocation_size_in_bytes;
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


                if (l_list_iterator->second._page_iterator > l_list_iterator->second._end)
                {
                    create_pages(region_name_p, 1);
                    continue;
                }

                return std::unique_ptr<U, deleter_type<U>>{static_cast<U*>(l_allocation_result), deleter_type<U>{reinterpret_cast<chunk_type*>(&(l_list_iterator->second)), size_p, l_queried_allocation_size_in_bytes}};
            }
            else
            {
                create_pages(region_name_p, 1);
            }
        }

        create_pages(region_name_p, 1);
        return allocate<U>(region_name_p, size_p);
    }

    _FORCE_INLINE_ static void create_pages(const char* region_name_p, size_t chunk_count_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        static chunk_type l_s_initial_value;
        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_memory_regions.emplace(region_name_p, l_s_initial_value);
        }
    }

    static void shrink_to_fit(const char* region_name_p) noexcept
    {
        index_t l_bucket_index = tl_s_memory_regions.bucket(region_name_p);
        typename namespace_pool_type::iterator  l_list_iterator = tl_s_memory_regions.begin(l_bucket_index);
        typename namespace_pool_type::const_iterator l_cend = tl_s_memory_regions.cend(l_bucket_index);
        FE_ASSERT(l_list_iterator == l_cend, "Unable to shrink_to_fit() an empty pool.");

        /*
                  first contains the address of the memory block.
                  second contains the size of the memory block.
        */

        var::size_t l_unused_memory_size_in_bytes = 0;
        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            for (auto block : l_list_iterator->second._free_blocks)
            {
                l_unused_memory_size_in_bytes += block.second;
            }

            if (l_list_iterator->second._page_iterator < l_list_iterator->second._end)
            {
                l_unused_memory_size_in_bytes += l_list_iterator->second._end - l_list_iterator->second._page_iterator;
            }
        }

        if (l_unused_memory_size_in_bytes == (ChunkCapacity * tl_s_memory_regions.bucket_size(l_bucket_index)))
        {
            tl_s_memory_regions.erase(tl_s_memory_regions.find(region_name_p));
        }
    }

    template <typename T>
    static void deallocate(const char* region_name_p, T* pointer_p, count_t element_count_p) noexcept
    {
        index_t l_bucket_index = tl_s_memory_regions.bucket(region_name_p);
        typename namespace_pool_type::iterator  l_list_iterator = tl_s_memory_regions.begin(l_bucket_index);
        typename namespace_pool_type::const_iterator l_cend = tl_s_memory_regions.cend(l_bucket_index);
        FE_ASSERT(l_list_iterator == l_cend, "Unable to request deallocate() to an empty pool.");


        std::byte* const l_value = reinterpret_cast<std::byte*>(pointer_p);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if ((l_list_iterator->second._begin <= l_value) && (l_value < l_list_iterator->second._end))
            {
                if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                {
                    for (var::count_t i = 0; i < element_count_p; ++i)
                    {
                        pointer_p->~T();
                        ++pointer_p;
                    }
                }

                auto l_insertion_result = l_list_iterator->second._free_blocks.insert(block_info_type{ l_value, FE::calculate_aligned_memory_size_in_bytes<T, Alignment>(element_count_p) });
                FE_EXIT(l_insertion_result.second == false, FE::MEMORY_ERROR_1XX::_FATAL_ERROR_DOUBLE_FREE, "Double-free detected.");

                if (l_list_iterator->second._free_blocks.size() > 1)
                {
                    __merge(l_insertion_result.first, l_list_iterator->second._free_blocks);
                }
                return;
            }
        }
    }

private:
    /*
    FE.generic_pool

    allocation:
        Best -  O(1)+ (Amortised)
        Worst - O(N)

    deallocation:
        Best - O(2 * log N)+ (Amortised)
        Worst - O(N * log N)
    */
    static void __merge(recycler_iterator recently_deleted_p, recycler_type& free_block_list_p) noexcept
    {
        /*
                        first contains the address of the memory block.
                        second contains the size of the memory block.
        */

        auto l_cend = free_block_list_p.cend();
        auto l_next_block_address = recently_deleted_p->first + recently_deleted_p->second;
        auto l_result = free_block_list_p.find(l_next_block_address);

        while (l_cend != l_result)
        {
            recently_deleted_p->second += l_result->second;
            l_next_block_address = l_result->first + l_result->second;
            free_block_list_p.erase(l_result);
            l_result = free_block_list_p.find(l_next_block_address);
        }

  //      while (free_block_list_p.size() > 1)
  //      {
  //          auto l_prev = free_block_list_p.top();
  //          free_block_list_p.pop();
  //          
  //          auto l_next = free_block_list_p.top();
  //      
  //          FE_ASSERT(l_prev._address >= l_next._address, "${%s@0}: The priority queue has illegal address order. ${%s@1} always has lower address value than ${%s@2}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_HEAP_CORRUPTION), TO_STRING(l_prev._address), TO_STRING(l_next._address));
  //          FE_ASSERT(l_prev._address + l_prev._size_in_bytes > l_next._address, "${%s@0}: Free-ed memory block range collision detected!\nPlease check if the count value passed to generic_pool<>::deallocate<T>(T* ptr_p, count_t element_count_p) was incorrect or not.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_HEAP_CORRUPTION));

  //          if ((l_prev._address + l_prev._size_in_bytes) == l_next._address)
  //          {
  //              free_block_list_p.pop();
  //              free_block_list_p.push({ l_prev._address, l_prev._size_in_bytes + l_next._size_in_bytes });
  //          }
  //          else
  //          { 
  //              temporary_storage_p.push(l_prev);
  //          }
  //      }
  //     
  //      while(temporary_storage_p.is_empty() == false)
		//{
		//	free_block_list_p.push(std::move(temporary_storage_p.pop()));
		//}
    }

    template <typename T>
    static void __recycle(internal::pool::block_info<void, POOL_TYPE::_GENERIC>& out_memblock_info_p, chunk_type& memory_p, size_t queried_allocation_size_in_bytes_p) noexcept
    {
        FE_ASSERT(memory_p._free_blocks.is_empty() == true, "Assertion Failure: Cannot recycle from an empty bin.");

        /*
                     first contains the address of the memory block.
                     second contains the size of the memory block.
        */

        auto l_cend = memory_p._free_blocks.cend();

        for (auto free_block_iterator = memory_p._free_blocks.begin(); free_block_iterator != l_cend; ++free_block_iterator)
        {
            if (free_block_iterator->second >= queried_allocation_size_in_bytes_p)
            {
                out_memblock_info_p._address = free_block_iterator->first;
                out_memblock_info_p._size_in_bytes = queried_allocation_size_in_bytes_p;
                auto l_leftover_address = free_block_iterator->first + queried_allocation_size_in_bytes_p;
                auto l_leftover_size = free_block_iterator->second - queried_allocation_size_in_bytes_p;
                
                if (l_leftover_size > 0)
                {
                    _MAYBE_UNUSED_ auto l_insertion_result = memory_p._free_blocks.insert(block_info_type{ l_leftover_address, l_leftover_size });
                    FE_EXIT(l_insertion_result.second == false, FE::MEMORY_ERROR_1XX::_FATAL_ERROR_DOUBLE_FREE, "Double-free detected.");
                }
                memory_p._free_blocks.erase(free_block_iterator);
                return;
            }
        }

       /* out_memblock_info_p = memory_p._free_blocks.top();
        while (out_memblock_info_p._size_in_bytes < queried_allocation_size_in_bytes_p)
        {
            deleter_type<T>::base_type::tl_s_temporary_storage.push(std::move(out_memblock_info_p));
            memory_p._free_blocks.pop();

            if (memory_p._free_blocks.is_empty() == true)
            {
                break;
            }
            else
            {
                out_memblock_info_p = memory_p._free_blocks.top();
            }
        }

        if (memory_p._free_blocks.is_empty() == false)
        {
            memory_p._free_blocks.pop();
        }

        int64 l_remaining_size = (out_memblock_info_p._size_in_bytes - queried_allocation_size_in_bytes_p);
        if (l_remaining_size > 0)
        {
            memory_p._free_blocks.push(block_info_type{ (out_memblock_info_p._address + queried_allocation_size_in_bytes_p), static_cast<size_t>(l_remaining_size) });
            out_memblock_info_p._size_in_bytes = queried_allocation_size_in_bytes_p;
        }

        while (deleter_type<T>::base_type::tl_s_temporary_storage.is_empty() == false)
        {
            memory_p._free_blocks.push(std::move(deleter_type<T>::base_type::tl_s_temporary_storage.pop()));
        }

        deleter_type<T>::base_type::tl_s_temporary_storage.pop_all();*/
    }
};


template<size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
thread_local typename pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::global_pool_type pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::tl_s_global_memory;

template<size_t ChunkCapacity, class Alignment, class GlobalAllocator, class NamespaceAllocator>
thread_local typename pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::namespace_pool_type pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>::tl_s_memory_regions;




template<typename T, size_t ChunkCapacity = 128, class GlobalAllocator = FE::scalable_aligned_allocator<internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>>, class NamespaceAllocator = FE::scalable_aligned_allocator<std::pair<const FE::memory_region_t, internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>>>>
using block_pool = pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>, GlobalAllocator, NamespaceAllocator>;

template<typename T, size_t ChunkCapacity = 128, class GlobalAllocator = FE::scalable_aligned_allocator<internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>>, class NamespaceAllocator = FE::scalable_aligned_allocator<std::pair<const FE::memory_region_t, internal::pool::chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>>>>
using block_pool_ptr = std::unique_ptr<T, pool_deleter<T, FE::POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>, GlobalAllocator, NamespaceAllocator>>;


template<size_t ChunkCapacity = 512 MB, class Alignment = FE::align_8bytes, class GlobalAllocator = FE::scalable_aligned_allocator<internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>>, class NamespaceAllocator = FE::scalable_aligned_allocator<std::pair<const FE::memory_region_t, internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>>>>
using generic_pool = pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>;

template<typename T, size_t ChunkCapacity = 512 MB, class Alignment = FE::align_8bytes, class GlobalAllocator = FE::scalable_aligned_allocator<internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>>, class NamespaceAllocator = FE::scalable_aligned_allocator<std::pair<const FE::memory_region_t, internal::pool::chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>>>>
using generic_pool_ptr = std::unique_ptr<T, pool_deleter<T, FE::POOL_TYPE::_GENERIC, ChunkCapacity, Alignment, GlobalAllocator, NamespaceAllocator>>;


template<uint64 Capacity>
struct capacity final
{
    _MAYBE_UNUSED_ static constexpr inline size_t size = Capacity;
};


template<class PoolType>
class scoped_pool_resource final
{
public:
    using pool_type = PoolType;

    _FORCE_INLINE_ scoped_pool_resource(count_t pages_p) noexcept
    {
        pool_type::create_pages(pages_p);
    }

    scoped_pool_resource(const scoped_pool_resource& other_p) noexcept = delete;
    scoped_pool_resource(scoped_pool_resource&& rvalue_p) noexcept = delete;
    scoped_pool_resource& operator=(const scoped_pool_resource& other_p) noexcept = delete;
    scoped_pool_resource& operator=(scoped_pool_resource&& rvalue_p) noexcept = delete;

    _FORCE_INLINE_ ~scoped_pool_resource() noexcept
    {     
        pool_type::shrink_to_fit();
    }
};

template<class NamespacePoolType>
class scoped_namespace_pool_resource final
{
    const char* m_region_name;

public:
    using pool_type = NamespacePoolType;

    _FORCE_INLINE_ scoped_namespace_pool_resource(const char* region_name_p, count_t pages_p) noexcept : m_region_name(region_name_p)
    {
        pool_type::create_pages(region_name_p, pages_p);
    }

    scoped_namespace_pool_resource(const scoped_namespace_pool_resource& other_p) noexcept = delete;
    scoped_namespace_pool_resource(scoped_namespace_pool_resource&& rvalue_p) noexcept = delete;
    scoped_namespace_pool_resource& operator=(const scoped_namespace_pool_resource& other_p) noexcept = delete;
    scoped_namespace_pool_resource& operator=(scoped_namespace_pool_resource&& rvalue_p) noexcept = delete;

    _FORCE_INLINE_ ~scoped_namespace_pool_resource() noexcept
    {
        pool_type::shrink_to_fit(this->m_region_name);
    }
};


END_NAMESPACE
#pragma warning (pop)
#endif