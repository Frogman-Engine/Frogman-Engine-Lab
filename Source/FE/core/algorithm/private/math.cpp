#include <FE/core/algorithm/math.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.




BEGIN_NAMESPACE(FE::algorithm::math)


_CONSTEXPR17_ boolean is_prime(uint64 number_p) noexcept
{
	var::uint64 l_count = 2;
	while ((l_count < number_p) && (number_p % l_count) != 0)
	{
		++l_count;
	}

	return l_count == number_p;
}


_CONSTEXPR17_ uint64 to_prime(uint64 number_p) noexcept
{
	constexpr uint64 l_magical_seven = 7;


	switch (number_p)
	{
	case 0:
		_FALLTHROUGH_;
	case 1:
		return _SMALLEST_PRIME_NUMBER_;

	case 3:
		_FALLTHROUGH_;
	case 5:
		_FALLTHROUGH_;
	case 7:
		_FALLTHROUGH_;
	case 11:
		_FALLTHROUGH_;
	case 13:
		return number_p;

	default:
	{
		var::uint64 l_number = number_p;

		if (MODULO_BY_2(l_number) == 0)
		{
			l_number ^= l_magical_seven;

			if (MODULO_BY_2(l_number) == 0)
			{
				++l_number;
			}
		}

		while (is_prime(l_number) == false)
		{
			++l_number;
		}
		return l_number;
	}
	}
}


_CONSTEXPR17_ uint64 abs(int64 value_p) noexcept
{
	return ABS(value_p);
}

_CONSTEXPR17_ float64 abs(float64 value_p) noexcept
{
	return ABS(value_p);
}


_CONSTEXPR17_ index_t calculate_index(index_t row_p, index_t coordinate_x_p, index_t coordinate_y_p) noexcept
{
	return CALCULATE_INDEX(row_p, coordinate_x_p, coordinate_y_p);
}


_CONSTEXPR17_ boolean is_nearly_equal(float64 left_p, float64 right_p, float64 offset_p) noexcept
{
	return IS_NEARLY_EQUAL(left_p, right_p, offset_p);
}


END_NAMESPACE