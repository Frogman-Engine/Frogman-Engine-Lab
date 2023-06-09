﻿#include <gtest/gtest.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/heap_utilization.hpp>
#include <vector>
using namespace FE;


TEST(memcpy_s, _)
{
	constexpr auto l_length = 10;
	std::unique_ptr<int[]> l_source(new int [l_length]{1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
	std::unique_ptr<int[]> l_destination(new int [l_length]{0});

	FE::memcpy_s(l_destination.get(), l_length, sizeof(decltype(*l_destination.get())), l_source.get(), l_length, sizeof(decltype(*l_source.get())));

	for (int i = 0; i < l_length; ++i)
	{
		EXPECT_EQ(l_destination.get()[i], 1);
	}
}

TEST(memset_s, _)
{
	constexpr auto l_length = 40;
	std::unique_ptr<var::byte[]> l_pointer_array = std::make_unique<var::byte[]>(l_length);
	
	FE::memset_s(l_pointer_array.get(), 100, l_length, sizeof(var::byte));
		
	for (int i = 0; i < l_length; ++i)
	{
		EXPECT_EQ(l_pointer_array.get()[i], 100);
	}
}