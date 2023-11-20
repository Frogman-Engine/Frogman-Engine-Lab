#ifndef _FE_CORE_PAIR_HXX_
#define _FE_CORE_PAIR_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/macros/attributes.h>




BEGIN_NAMESPACE(FE)


template<typename First, typename Second>
struct pair
{
	using first_type = First;
	using second_type = Second;

	First _first;
	Second _second;

	_FORCE_INLINE_ pair() noexcept = default;
	_FORCE_INLINE_ pair(First first_p, Second second_p) noexcept : _first(std::move(first_p)), _second(std::move(second_p)) {};
	_FORCE_INLINE_ ~pair() noexcept = default;

	_FORCE_INLINE_ pair(const pair&) noexcept = default;
	_FORCE_INLINE_ pair(pair&& rvalue_p) noexcept : _first(std::move(rvalue_p._first)), _second(std::move(rvalue_p._second)) {};

	_FORCE_INLINE_ pair& operator=(const pair& other_p) noexcept
	{
		this->_first = other_p._first;
		this->_second = other_p._second;

		return *this;
	}

	_FORCE_INLINE_ pair& operator=(pair&& rvalue_p) noexcept
	{
		this->_first = std::move(rvalue_p._first);
		this->_second = std::move(rvalue_p._second);

		return *this;
	}
};




template<typename First, typename Second>
struct compressed_pair final
{
	using first_type = First;
	using second_type = Second;

	_NO_UNIQUE_ADDRESS_ First _first;
	_NO_UNIQUE_ADDRESS_ Second _second;

};


END_NAMESPACE
#endif