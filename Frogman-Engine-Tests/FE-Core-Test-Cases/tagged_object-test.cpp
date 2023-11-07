#include <gtest/gtest.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/miscellaneous/google_test_extension.h>
#include <FE/core/tagged_object.hxx>
#include <FE/core/algorithm/string.hxx>
using namespace FE;




TEST(tagged_object, construction)
{
	FE::tagged_object<int> l_first_tagged_object(5);
	FE::tagged_object<int> l_second_tagged_object(3, "FE::tagged_object<int>");

	FE::tagged_object<int> l_third_tagged_object = l_first_tagged_object;
	FE::tagged_object<int> l_fourth_tagged_object = std::move(l_second_tagged_object);

	EXPECT_EQ(l_first_tagged_object.copy_object(), 5);
}


TEST(tagged_object, assignment)
{
	FE::tagged_object<int> l_first_tagged_object;
	l_first_tagged_object = 5;
	l_first_tagged_object = "FE::tagged_object<int>";

	EXPECT_TRUE( l_first_tagged_object.copy_tag() == "FE::tagged_object<int>" );
}


TEST(tagged_object, compare)
{
	FE::tagged_object<int> l_first_tagged_object(5);
	FE::tagged_object<int> l_second_tagged_object(3, "FE::tagged_object<int>");

	EXPECT_TRUE(5 == l_first_tagged_object);
	EXPECT_TRUE("FE::tagged_object<int>" == l_second_tagged_object);
}


TEST(tagged_object, swap_and_compare)
{

	FE::tagged_object<int> l_first_tagged_object(5);
	FE::tagged_object<int> l_second_tagged_object(3);

	FE::tagged_object<int> l_third_tagged_object(5);

	l_second_tagged_object.swap(l_third_tagged_object);
	EXPECT_TRUE(l_first_tagged_object == l_second_tagged_object);
}