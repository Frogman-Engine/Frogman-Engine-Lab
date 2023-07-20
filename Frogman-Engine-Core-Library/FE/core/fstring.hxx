﻿#ifndef _FE_CORE_FSTRING_HXX_
#define _FE_CORE_FSTRING_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include "algorithm/string.hxx"
#include "iterator.hxx"
#include "private/memory.hpp"
#include "heap_utilization.hpp"


BEGIN_NAMESPACE(FE)


template<typename char_type, uint64 max_capacity>
class fixed_sized_string 
{
    static_assert(FE::is_character<char_type>::_VALUE_ == true, "char_type is not a valid character type");
    static_assert(FE::is_trivially_constructible_and_destructible<char_type>::_VALUE_ == FE::OBJECT_TRIVIALITY::_TRIVIAL, "char_type is not a valid character type");
   
    char_type m_fstring[max_capacity];
    var::uint64 m_string_length;

public:
    using value_type = char_type;
    using size_type = var::uint64;
    using length_type = var::uint64;
    using difference_type = var::ptrdiff_t;
    using reference = char_type&;
    using const_reference = const char_type&;
    using iterator = FE::iterator<FE::contiguous_iterator<char_type>>;
    using const_iterator = FE::const_iterator<FE::contiguous_iterator<char_type>>;
    using reverse_iterator = FE::reverse_iterator<FE::contiguous_iterator<char_type>>;
    using const_reverse_iterator = FE::const_reverse_iterator<FE::contiguous_iterator<char_type>>;

    constexpr fixed_sized_string() noexcept 
        : m_fstring{ _NULL_ }, m_string_length() {}

    constexpr fixed_sized_string(const char_type* const cstr_ptrc_p) noexcept 
        : m_fstring{ _NULL_ }, m_string_length(algorithm::string::string_length<char_type>(cstr_ptrc_p))
    {
        FE_ASSERT(this->m_string_length >= max_capacity, "CRITICAL ERROR: the input string length exceeds the max string capacity");
        algorithm::string::copy_string( this->m_fstring, max_capacity, cstr_ptrc_p, this->m_string_length);
    }

    constexpr fixed_sized_string(const fixed_sized_string<char_type, max_capacity>& other_cref_p) noexcept 
        : m_fstring{ _NULL_ }, m_string_length(other_cref_p.m_string_length)
    {
        algorithm::string::copy_string(this->m_fstring, max_capacity, other_cref_p.m_fstring, max_capacity);
    }

    constexpr fixed_sized_string(fixed_sized_string<char_type, max_capacity>&& rvalue_reference_p) noexcept 
        : m_fstring{ _NULL_ }, m_string_length(rvalue_reference_p.m_string_length)
    {
        algorithm::string::copy_string(this->m_fstring, max_capacity, rvalue_reference_p.m_fstring, max_capacity);
        UNALIGNED_MEMSET(rvalue_reference_p.m_fstring, _NULL_, rvalue_reference_p.m_string_length * sizeof(char_type));
        rvalue_reference_p.m_string_length = 0;
    }


    constexpr fixed_sized_string<char_type, max_capacity>& operator=(const char_type* const cstr_ptrc_p) noexcept
    {
        this->m_string_length = algorithm::string::string_length<char_type>(cstr_ptrc_p);

        FE_ASSERT(this->m_string_length >= max_capacity, "CRITICAL ERROR: the input string length exceeds the max string capacity");
        algorithm::string::copy_string(this->m_fstring, max_capacity, cstr_ptrc_p, this->m_string_length);
        return *this;
    }

    constexpr fixed_sized_string<char_type, max_capacity>& operator=(const fixed_sized_string<char_type, max_capacity>& other_cref_p) noexcept
    {
        this->m_string_length = other_cref_p.m_string_length;
        algorithm::string::copy_string(this->m_fstring, max_capacity, other_cref_p.m_fstring, max_capacity);
        return *this;
    }

    constexpr fixed_sized_string<char_type, max_capacity>& operator=(fixed_sized_string<char_type, max_capacity>&& rvalue_reference_p) noexcept
    {
        algorithm::string::copy_string(this->m_fstring, max_capacity, rvalue_reference_p.m_fstring, max_capacity);
        UNALIGNED_MEMSET(rvalue_reference_p.m_fstring, _NULL_, rvalue_reference_p.m_string_length * sizeof(char_type));
        rvalue_reference_p.m_string_length = 0;
        return *this;
    }

    constexpr char_type& operator[](index_t idx_p) noexcept
    {
        FE_ASSERT(idx_p >= this->m_string_length, "CRITICAL ERROR: index out of boundary");
        return this->m_fstring[idx_p];
    }

    constexpr char_type front() const noexcept { return *this->m_fstring; }
    constexpr char_type back() const noexcept { return this->m_fstring[this->m_string_length - 1]; }

    // It returns a pointer to the data
    constexpr char_type* data() noexcept { return this->m_fstring; }

    // It returns a pointer to a traditional c-style read-only string
    constexpr const char_type* c_str() const noexcept { return this->m_fstring; }

    // It returns a standard string
    constexpr std::basic_string<char_type> std_str() const noexcept { return this->m_fstring; }

    constexpr iterator begin() noexcept { return this->m_fstring; }
    constexpr iterator end() noexcept { return this->m_fstring + this->m_string_length; }
    constexpr reverse_iterator rbegin() noexcept { return this->m_fstring + (this->m_string_length - 1); }
    constexpr reverse_iterator rend() noexcept { return this->m_fstring - 1; }
    constexpr const_iterator cbegin() noexcept { return this->m_fstring; }
    constexpr const_iterator cend() noexcept { return this->m_fstring + this->m_string_length; }
    constexpr const_reverse_iterator crbegin() noexcept { return this->m_fstring + (this->m_string_length - 1); }
    constexpr const_reverse_iterator crend() noexcept { return this->m_fstring - 1; }

    constexpr bool is_empty() const noexcept
    {
        return (this->m_string_length == 0) ? true : false;
    }

    _FORCE_INLINE_ constexpr length_type length() const noexcept { return this->m_string_length; }
    _FORCE_INLINE_ constexpr length_type max_length() const noexcept { return max_capacity; }
    _FORCE_INLINE_ constexpr size_type capacity() const noexcept { return max_capacity; }

    _FORCE_INLINE_ constexpr void clear() noexcept { UNALIGNED_MEMSET(this->m_fstring, _NULL_, max_capacity * sizeof(char_type)); }
    // insert
    // erase

    constexpr void push_back(const char_type char_type_element_p) noexcept
    {
        FE_ASSERT(this->m_string_length >= max_capacity, "CRITICAL ERROR: out of capacity");
        this->m_fstring[this->m_string_length] = char_type_element_p;
        ++(this->m_string_length);
    }
    constexpr char_type pop_back() noexcept
    {
        const char_type l_return_value = this->back();
        --this->m_string_length;
        this->m_fstring[this->m_string_length] = static_cast<const char_type>('\0');
        return l_return_value;
    }

    // append
    constexpr fixed_sized_string<char_type, max_capacity>& operator+=(const fixed_sized_string<char_type, max_capacity>& other_cref_p) noexcept
    {
        this->m_string_length += other_cref_p.m_string_length;
        algorithm::string::string_concatenation<char_type>(this->m_fstring, max_capacity, other_cref_p.m_fstring, other_cref_p.m_string_length);
        return *this;
    }
    constexpr fixed_sized_string<char_type, max_capacity>& operator+=(const char_type* const c_style_string_ptrc_p) noexcept
    {
        length_type l_input_string_length = algorithm::string::string_length<char_type>(c_style_string_ptrc_p);
        algorithm::string::string_concatenation<char_type>(this->m_fstring, max_capacity, c_style_string_ptrc_p, l_input_string_length);
        this->m_string_length += l_input_string_length;
        return *this;
    }

    constexpr std::optional<algorithm::string::range> find(const char_type* const cstr_ptrc_p) const noexcept
    {
        return algorithm::string::search_very_first_substring(this->m_fstring, cstr_ptrc_p);
    }
    constexpr std::optional<algorithm::string::range> find(fixed_sized_string<char_type, max_capacity>& sub_basic_cstring_ref_p) const noexcept
    {
        return algorithm::string::search_very_first_substring(this->m_fstring, sub_basic_cstring_ref_p.m_fstring);
    }

    constexpr var::boolean starts_with(const char_type* const cstr_ptrc_p) const noexcept { return false; }
    constexpr var::boolean starts_with(fixed_sized_string<char_type, max_capacity>& sub_basic_cstring_ref_p) const noexcept { return false; }

    constexpr var::boolean ends_with(const char_type* const cstr_ptrc_p) const noexcept { return false; }
    constexpr var::boolean ends_with(fixed_sized_string<char_type, max_capacity>& sub_basic_cstring_ref_p) const noexcept { return false; }

    constexpr void copy_substring(index_t begin_p, index_t end_p, char_type* const in_out_buffer_ptrc_p, capacity_t in_out_buffer_capacity_p) const noexcept
    {
        algorithm::string::copy_string(in_out_buffer_ptrc_p, in_out_buffer_capacity_p, this->m_fstring + begin_p, end_p - begin_p);
    }
    constexpr void copy_substring(index_t begin_p, index_t end_p, fixed_sized_string<char_type, max_capacity>& in_out_buffer_basic_cstring_ref_p) const noexcept 
    {
        algorithm::string::copy_string(in_out_buffer_basic_cstring_ref_p.m_fstring, max_capacity, this->m_fstring + begin_p, end_p - begin_p);
    }

    constexpr _FORCE_INLINE_ static void swap(fixed_sized_string<char_type, max_capacity>& first_ref_p, fixed_sized_string<char_type, max_capacity>& second_ref_p) noexcept
    {
        fixed_sized_string<char_type, max_capacity> l_temp = std::move(first_ref_p);
        first_ref_p = std::move(second_ref_p);
        second_ref_p = std::move(l_temp);
    }

    constexpr boolean operator==(fixed_sized_string<char_type, max_capacity>& other_ref_p) noexcept
    {
        return algorithm::string::string_comparison(this->m_fstring, other_ref_p.m_fstring);
    }
    constexpr boolean operator==(const char_type* const cstr_ptrc_p) noexcept
    {
        return algorithm::string::string_comparison(this->m_fstring, cstr_ptrc_p);
    }

    constexpr boolean operator!=(fixed_sized_string<char_type, max_capacity>& other_ref_p) noexcept
    {
        return !algorithm::string::string_comparison(this->m_fstring, other_ref_p.m_fstring);
    }
    constexpr boolean operator!=(const char_type* const cstr_ptrc_p) noexcept
    {
        return !algorithm::string::string_comparison(this->m_fstring, cstr_ptrc_p);
    }

    constexpr friend boolean operator==(const char_type* const cstr_ptrc_p, fixed_sized_string<char_type, max_capacity>& fstring_ref_p) noexcept
    {
        return algorithm::string::string_comparison(cstr_ptrc_p, fstring_ref_p.m_fstring);
    }
    constexpr friend boolean operator!=(const char_type* const cstr_ptrc_p, fixed_sized_string<char_type, max_capacity>& fstring_ref_p) noexcept
    {
        return !algorithm::string::string_comparison(cstr_ptrc_p, fstring_ref_p.m_fstring);
    }
};


template<uint64 max_capacity>
using fstring = FE::fixed_sized_string<char, max_capacity>;
template<uint64 max_capacity>
using ufstring = FE::fixed_sized_string<var::uchar, max_capacity>;
template<uint64 max_capacity>
using sfstring = FE::fixed_sized_string<var::schar, max_capacity>;

template<uint64 max_capacity>
using fwstring = FE::fixed_sized_string<var::wchar, max_capacity>;

#if _HAS_CXX20_ == 1
template<uint64 max_capacity>
using fstring8 = FE::fixed_sized_string<var::UTF8, max_capacity>;
#endif

template<uint64 max_capacity>
using fstring16 = FE::fixed_sized_string<var::UTF16, max_capacity>;

template<uint64 max_capacity>
using fstring32 = FE::fixed_sized_string<var::UTF32, max_capacity>;


END_NAMESPACE;
#endif
