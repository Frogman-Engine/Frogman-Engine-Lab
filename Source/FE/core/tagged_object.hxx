#ifndef _FE_CORE_TAGGED_OBJECT_HXX_
#define _FE_CORE_TAGGED_OBJECT_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/algorithm/utility.hxx>
#include <FE/core/memory.hxx>
#include <string_view>




BEGIN_NAMESPACE(FE)


#pragma pack(push, dword_size)

template <typename T, class Alignment = ::FE::align_8bytes, class TagType = std::string_view>
class alignas(Alignment::size) tagged_object final
{
	TagType m_tag;
	T m_object;

public:
	typedef T value_type;
	typedef Alignment alignment_type;
	typedef TagType tag_type;

	_CONSTEXPR17_ tagged_object() noexcept : m_tag("\0"), m_object() {}
	_CONSTEXPR17_ tagged_object(T object_p, TagType tag_p) noexcept : m_tag(std::move(tag_p)), m_object(std::move(object_p)) {}
	_CONSTEXPR17_ tagged_object(T object_p) noexcept : m_tag("\0"), m_object(std::move(object_p)) {}
	

	_CONSTEXPR17_ tagged_object(tagged_object<T, Alignment, TagType>& other_p) noexcept
	{
		this->m_object = other_p.m_object;
		this->m_tag = other_p.m_tag;
	}


	_CONSTEXPR17_ tagged_object(tagged_object<T, Alignment, TagType>&& rvalue_p) noexcept
	{
		this->m_object = rvalue_p.m_object;
		this->m_tag = rvalue_p.m_tag;
	}


	_CONSTEXPR17_ ~tagged_object() noexcept {};


	_CONSTEXPR17_ T& operator=(T value_p) noexcept
	{
		this->m_object = ::std::move(value_p);
		return this->m_object;
	}
	
	_CONSTEXPR17_ TagType& operator=(TagType tag_p) noexcept
	{
		this->m_tag = ::std::move(tag_p);
		return this->m_tag;
	}


	_CONSTEXPR17_ tagged_object& operator=(tagged_object<T, Alignment, TagType>& other_p) noexcept
	{
		this->m_object = other_p.m_object;
		this->m_tag = other_p.m_tag;

		return *this;
	}
	_CONSTEXPR17_ tagged_object& operator=(tagged_object<T, Alignment, TagType>&& rvalue_p) noexcept
	{
		this->m_object = rvalue_p.m_object;
		this->m_tag = rvalue_p.m_tag;

		return *this;
	}

	_FORCE_INLINE_ T copy_object() const noexcept { return this->m_object; }
	_FORCE_INLINE_ TagType copy_tag() const noexcept { return this->m_tag; }


	_FORCE_INLINE_ boolean operator==(TagType tag_p) const noexcept
	{
		return this->m_tag == tag_p ? true : false;
	}
	_FORCE_INLINE_ boolean operator==(T value_p) const noexcept
	{
		return this->m_object == value_p ? true : false;
	}
	_FORCE_INLINE_ boolean operator!=(TagType tag_p) const noexcept
	{
		return this->m_tag != tag_p ? true : false;
	}
	_FORCE_INLINE_ boolean operator!=(T value_p) const noexcept
	{
		return this->m_object != value_p ? true : false;
	}

	_FORCE_INLINE_ boolean operator==(TagType& tag_p) const noexcept
	{
		return this->m_tag == tag_p ? true : false;
	}
	_FORCE_INLINE_ boolean operator==(T& value_p) const noexcept
	{
		return this->m_object == value_p ? true : false;
	}
	_FORCE_INLINE_ boolean operator!=(TagType& tag_p) const noexcept
	{
		return this->m_tag != tag_p ? true : false;
	}
	_FORCE_INLINE_ boolean operator!=(T& value_p) const noexcept
	{
		return this->m_object != value_p ? true : false;
	}


	_FORCE_INLINE_ friend boolean operator==(T value_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return value_p == element_p.m_object ? true : false;
	}
	_FORCE_INLINE_ friend boolean operator!=(T value_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return value_p != element_p.m_object ? true : false;
	}
	_FORCE_INLINE_ friend boolean operator==(T& value_ref_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return value_ref_p == element_p.m_object ? true : false;
	}
	_FORCE_INLINE_ friend boolean operator!=(T& value_ref_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return value_ref_p != element_p.m_object ? true : false;
	}
	_FORCE_INLINE_ friend boolean operator==(TagType tag_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return tag_p == element_p.m_tag ? true : false;
	}
	_FORCE_INLINE_ friend boolean operator!=(TagType tag_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return tag_p != element_p.m_tag ? true : false;
	}
	_FORCE_INLINE_ friend boolean operator==(TagType& tag_ref_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return tag_ref_p == element_p.m_tag ? true : false;
	}
	_FORCE_INLINE_ friend boolean operator!=(TagType& tag_ref_p, tagged_object<T, Alignment, TagType>& element_p) noexcept
	{
		return tag_ref_p != element_p.m_tag ? true : false;
	}

	_FORCE_INLINE_ boolean operator==(tagged_object<T, Alignment, TagType>& element_p) const noexcept
	{
		if (this->m_object != element_p.m_object)
		{
			return false;
		}

		if (this->m_tag != element_p.m_tag)
		{
			return false;
		}

		return true;
	}
	_FORCE_INLINE_ boolean operator!=(tagged_object<T, Alignment, TagType>& element_p) const noexcept
	{
		if (this->m_object == element_p.m_object)
		{
			return false;
		}

		if (this->m_tag == element_p.m_tag)
		{
			return false;
		}

		return true;
	}

	_FORCE_INLINE_ void swap(tagged_object& in_out_other_p) noexcept
	{
		algorithm::utility::swap(*this, in_out_other_p);
	}
};

#pragma pack(pop, dword_size)


END_NAMESPACE
#endif 
