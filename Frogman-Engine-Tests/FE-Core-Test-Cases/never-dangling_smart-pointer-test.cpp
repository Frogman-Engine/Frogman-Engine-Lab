#include <gtest/gtest.h>
#include <FE/core/smart_pointers/smart_ptrs.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.




TEST(smart_ptr_variants, never_dangling_pointer)
{
	using namespace FE;

	FE::proxy_ptr<var::byte[]> l_smart_proxy_pointer;
	EXPECT_TRUE(l_smart_proxy_pointer.is_expired()); // The proxy pointer is pointing to nothing.

	{
		FE::exclusive_ptr<var::byte[]> l_smart_array_pointer = FE::make_exclusive<var::byte[]>(FE::size_t{ 128 });
		l_smart_proxy_pointer = l_smart_array_pointer;

		EXPECT_FALSE(l_smart_proxy_pointer.is_expired());

		l_smart_array_pointer.reset();  // deletes the memory to reset
		l_smart_array_pointer.reset(FE::resize_to{ 64 }); // The target object address has been changed.

		let boolean L_is_expired = l_smart_proxy_pointer.is_expired(); 
		// Checks if the exclusive_ptr is invalidated, and it updates the internal pointer to avoid the dangling pointer issue (a pointer to an invalid address).
		EXPECT_FALSE(L_is_expired);
		if (L_is_expired == false)
		{
			_MAYBE_UNUSED_ auto l_will_do_something_with_this_return_value = l_smart_proxy_pointer.get_unchecked();
			// operator->() and operator*() can be expensive in certain circumstances, such as loops and performance-critical contexts.
			// something happens below...
		}
	} // The exclusive pointer dies at this point.

	EXPECT_TRUE(l_smart_proxy_pointer.is_expired());  // Cannot proxy a dead exclusive pointer to indirect the target object.
}




TEST(std_smart_ptrs, dangling_pointer)
{
	std::shared_ptr<std::byte[]> l_shared = std::make_shared<std::byte[]>(16);
	std::weak_ptr<std::byte[]>l_watcher = l_shared; // The weak pointer is pointing to the sixteen bytes sized memory.

	{
		std::shared_ptr<std::byte[]> l_new_capacity = std::make_shared<std::byte[]>(64);
		l_shared.swap(l_new_capacity); // It swaps to extend the size.
	}

	EXPECT_TRUE(l_watcher.expired()); // the address changed, the weak pointer is dangling now.
}