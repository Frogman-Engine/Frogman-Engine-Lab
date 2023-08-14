﻿#ifndef _FE_CORE_CONCURRENT_MEMORY_BLOCK_HXX_
#define _FE_CORE_CONCURRENT_MEMORY_BLOCK_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/memory.hxx>
#include "movable_scoped_ref.hxx"




BEGIN_NAMESPACE(FE)


#pragma pack(push, _DWORD_SIZE_)
// construction and destruction operations are lock-free and thread-safe but the instance may not be safe from data race condition.
template<typename T, class alignment = FE::align_null>
class alignas(alignment::size) concurrent_memory_block final
{
private:
	internal::IN_USE m_is_being_used;
	::std::atomic_bool m_is_block_constructed;
	var::byte m_memory[sizeof(T)];
	T* const m_memory_ptrc;

	thread_local static T tl_s_null_object;

public: 
	typedef T value_type;
	typedef alignment alignment_type;

	_FORCE_INLINE_ constexpr concurrent_memory_block() noexcept : m_is_being_used(false), m_is_block_constructed(false), m_memory(), m_memory_ptrc(reinterpret_cast<T*>(m_memory)) {}

	_FORCE_INLINE_ ~concurrent_memory_block()
	{
		if (this->m_is_block_constructed.load(std::memory_order_acquire) == true)
		{
			this->m_memory_ptrc->~T();
		}
	}

	_FORCE_INLINE_ concurrent_memory_block(const concurrent_memory_block& other_cref_p) noexcept : m_is_block_constructed(other_cref_p.m_is_block_constructed), m_memory(), m_memory_ptrc(reinterpret_cast<T*>(m_memory))
	{
		if (other_cref_p.m_is_block_constructed.load(std::memory_order_acquire) == true)
		{
			new(this->m_memory_ptrc) T();
			*this->m_memory_ptrc = *other_cref_p.m_memory_ptrc;
		}
	}

	_FORCE_INLINE_ concurrent_memory_block(concurrent_memory_block&& rvalue_p) noexcept : m_is_block_constructed(rvalue_p.m_is_block_constructed), m_memory(), m_memory_ptrc(reinterpret_cast<T*>(m_memory))
	{
		if (rvalue_p.m_is_block_constructed.load(std::memory_order_acquire) == false)
		{
			return;
		}

		new(this->m_memory_ptrc) T();
		*this->m_memory_ptrc = std::move(*rvalue_p.m_memory_ptrc);
	}

	_FORCE_INLINE_ concurrent_memory_block& operator=(const concurrent_memory_block& other_cref_p) noexcept
	{
		if (other_cref_p.m_is_block_constructed.load(std::memory_order_acquire) == false)
		{
			return *this;
		}

		if (this->m_is_block_constructed.load(std::memory_order_acquire) == false)
		{
			new(this->m_memory_ptrc) T();
			this->m_is_block_constructed.store(true, std::memory_order_release);
		}

		*this->m_memory_ptrc = *other_cref_p.m_memory_ptrc;

		return *this;
	}

	_FORCE_INLINE_ concurrent_memory_block& operator=(concurrent_memory_block&& rvalue_p) noexcept
	{
		if (rvalue_p.m_is_block_constructed.load(std::memory_order_acquire) == false)
		{
			return *this;
		}

		if (this->m_is_block_constructed.load(std::memory_order_acquire) == false)
		{
			new(this->m_memory_ptrc) T();
			this->m_is_block_constructed.store(true, std::memory_order_release);
		}

		*this->m_memory_ptrc = std::move(*rvalue_p.m_memory_ptrc);

		return *this;
	}

	_NODISCARD_ _FORCE_INLINE_ movable_scoped_ref<T> operator*() noexcept
	{
		FE_ASSERT(this->m_is_block_constructed.load(std::memory_order_acquire) == false, "${%s@0}: Attempted to dereference an uninitialized memory block.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		return std::move(movable_scoped_ref<T>(this->m_memory_ptrc, &this->m_is_being_used, true));
	}

	_NODISCARD_ _FORCE_INLINE_ movable_scoped_ref<T> try_access() noexcept
	{
		if (FE_EXCEPTION_LOG(this->m_is_block_constructed.load(std::memory_order_acquire) == false, "WARNING: failed to access the memory block because the block is not constructed. The function will return early with a null object."))
		{
			return movable_scoped_ref<T>(&concurrent_memory_block<T, alignment>::tl_s_null_object, nullptr, false);
		}

		return std::move(movable_scoped_ref<T>(this->m_memory_ptrc, &this->m_is_being_used, true));
	}

public:
	_FORCE_INLINE_ var::boolean is_constructed() const noexcept { return this->m_is_block_constructed.load(std::memory_order_acquire); }

	_FORCE_INLINE_ var::boolean call_constructor() noexcept
	{
		boolean l_is_constructed = this->m_is_block_constructed.exchange(true, std::memory_order_acq_rel);
		if (l_is_constructed == false)
		{
			new(this->m_memory_ptrc) T();
		}
		return !l_is_constructed;
	}

	_FORCE_INLINE_ var::boolean call_copy_constructor(T& other_ref_p) noexcept
	{
		boolean l_is_constructed = this->m_is_block_constructed.exchange(true, std::memory_order_acq_rel);
		if (l_is_constructed == false)
		{
			new(this->m_memory_ptrc) T(other_ref_p);
		}
		return !l_is_constructed;
	}

	_FORCE_INLINE_ var::boolean call_move_constructor(T&& rvalue_p) noexcept
	{
		boolean l_is_constructed = this->m_is_block_constructed.exchange(true, std::memory_order_acq_rel);
		if (l_is_constructed == false)
		{
			new(this->m_memory_ptrc) T(std::move(rvalue_p));
		}
		return !l_is_constructed;
	}

	template<typename ... arguments>
	_FORCE_INLINE_ var::boolean call_constructor(arguments&& ...arguments_p) noexcept
	{
		boolean l_is_constructed = this->m_is_block_constructed.exchange(true, std::memory_order_acq_rel);
		if (l_is_constructed == false)
		{
			new(this->m_memory_ptrc) T(arguments_p...);
		}
		return !l_is_constructed;
	}

	_FORCE_INLINE_ var::boolean call_destructor() noexcept
	{
		boolean l_is_constructed = this->m_is_block_constructed.exchange(false, std::memory_order_acq_rel);
		if (l_is_constructed == true && this->m_is_being_used == false)
		{
			this->m_memory_ptrc->~T();
			return _SUCCESSFUL_;
		}
		
		this->m_is_block_constructed.store(true, std::memory_order_release);
		return _FAILED_;
	}

	_FORCE_INLINE_ var::boolean is_being_used() noexcept { return this->m_is_being_used.load(::std::memory_order_acquire); }

};

template<typename T, class alignment>
thread_local T concurrent_memory_block<T, alignment>::tl_s_null_object;

#pragma pack(pop, _DWORD_SIZE_)

END_NAMESPACE
#endif
