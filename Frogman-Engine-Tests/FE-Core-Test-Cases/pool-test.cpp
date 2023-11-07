#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <FE/core/pool.hxx>
#include <FE/core/pool_allocator.hxx>
#include <FE/core/algorithm/string.hxx>

#include <FE/miscellaneous/undefine_max_min.h>
#include <boost/pool/pool_alloc.hpp>
#include <FE/miscellaneous/define_max_min.h>

#include <string>




TEST(pool, block_allocation)
{
	FE::block_pool<std::string>::create_pages(1);

	{
		FE::block_pool_ptr<std::string> l_smart_ptr = FE::block_pool<std::string>::allocate();

		l_smart_ptr->reserve(20);
		l_smart_ptr->assign("Memory Pool!");

		EXPECT_TRUE(FE::algorithm::string::compare(l_smart_ptr->c_str(), "Memory Pool!"));
	}


	FE::block_pool<std::string>::create_pages(1, FE::memory_region_t{ "Restaurant" });

	{
		FE::block_pool_ptr<std::string> l_smart_ptr1 = FE::block_pool<std::string>::allocate(FE::memory_region_t{ "Restaurant" });
		l_smart_ptr1->reserve(20);
		l_smart_ptr1->assign("Aglio e olio!");

		FE::block_pool_ptr<std::string> l_smart_ptr2 = FE::block_pool<std::string>::allocate(FE::memory_region_t{ "Restaurant" });
		l_smart_ptr2->reserve(20);
		l_smart_ptr2->assign("Pizza!");

		FE::block_pool_ptr<std::string> l_smart_ptr3 = FE::block_pool<std::string>::allocate(FE::memory_region_t{ "Restaurant" });
		l_smart_ptr3->reserve(20);
		l_smart_ptr3->assign("Vongole!");

		FE::block_pool_ptr<std::string> l_smart_ptr4 = FE::block_pool<std::string>::allocate(FE::memory_region_t{ "Restaurant" });
		l_smart_ptr4->reserve(20);
		l_smart_ptr4->assign("Carbonara!");

		FE::block_pool_ptr<std::string> l_smart_ptr5 = FE::block_pool<std::string>::allocate(FE::memory_region_t{ "Restaurant" });
		l_smart_ptr5->reserve(20);
		l_smart_ptr5->assign("Arrabbiata!");
	}


	{
		FE::block_pool_ptr<FE::var::int64> l_smart_ptr[10];

		for (FE::var::uint32 i = 0; i < 10; ++i)
		{
			l_smart_ptr[i] = FE::block_pool<FE::var::int64>::allocate();
		}


	}

	
	
	FE::block_pool<std::string>::shrink_to_fit();
	FE::block_pool<std::string>::shrink_to_fit(FE::memory_region_t{ "Restaurant" });
	FE::block_pool<FE::var::int64>::shrink_to_fit();
}




class mesh
{
};

class animaiton
{
};

class mags
{
};

class player_character
{
	FE::generic_pool_ptr<animaiton> m_animation;
	FE::generic_pool_ptr<mesh> m_mesh;
	FE::generic_pool_ptr<mags> m_mags;

public:
	player_character() noexcept
	{
		FE::generic_pool<>::create_pages(1, { "Client Player Character's Properties" });

		this->m_animation = FE::generic_pool<>::allocate<animaiton>(1, { "Client Player Character's Properties" });
		this->m_mesh = FE::generic_pool<>::allocate<mesh>(1, { "Client Player Character's Properties" });
		this->m_mags = FE::generic_pool<>::allocate<mags>(10, { "Client Player Character's Properties" });
	}

	~player_character() noexcept = default;
};

TEST(pool, generic_block_allocation)
{
	FE::generic_pool<>::create_pages(1);

	{
		FE::generic_pool_ptr<std::vector<FE::var::int32>> l_smart_ptr = FE::generic_pool<>::allocate<std::vector<FE::var::int32>>();
	}

	{
		FE::generic_pool_ptr<std::string> l_smart_ptr = FE::generic_pool<>::allocate<std::string>();
	}


	{
		player_character l_my_character;
	}

	{
		player_character l_my_character;
	}

	FE::generic_pool<>::shrink_to_fit({ "Client Player Character's Properties" });

	{
		FE::generic_pool_ptr<FE::var::int64> l_smart_ptr[8];

		for (FE::var::uint32 i = 0; i < 8; ++i)
		{
			l_smart_ptr[i] = FE::generic_pool<>::allocate<FE::var::int64>();
		}

		FE::generic_pool_ptr<FE::var::int64> l_another_smart_ptr;
		l_another_smart_ptr = std::move(l_smart_ptr[0]);
	}

	{
		FE::generic_pool_ptr<FE::var::int64> l_smart_ptr[4];

		for (FE::var::uint32 i = 0; i < 4; ++i)
		{
			l_smart_ptr[i] = FE::generic_pool<>::allocate<FE::var::int64>();
		}

		FE::generic_pool_ptr<FE::var::int64> l_another_smart_ptr = std::move(l_smart_ptr[0]);
	}
}




TEST(new_delete_pool_allocator, all)
{
	{
		auto l_ptr = FE::new_delete_pool_allocator<std::string>::allocate(1);

		l_ptr = FE::new_delete_pool_allocator<std::string>::reallocate(l_ptr, 1, 2);

		FE::new_delete_pool_allocator<std::string>::deallocate(l_ptr, 2);
	}

	{
		FE::std_style::new_delete_pool_allocator<std::string> l_allocator;

		auto l_ptr = l_allocator.allocate(1);

		l_ptr = l_allocator.reallocate(l_ptr, 1, 2);

		l_allocator.deallocate(l_ptr, 2);
	}
}




void FE_block_pool(benchmark::State& state_p) noexcept
{
	FE::block_pool<std::string>::create_pages(1);

	for(auto _ : state_p)
	{
		FE::block_pool_ptr<std::string> l_smart_ptr = FE::block_pool<std::string>::allocate();
	}
}
BENCHMARK(FE_block_pool);


void FE_block_region_pool(benchmark::State& state_p) noexcept
{
	FE::block_pool<std::string>::create_pages(1, { "Area B" });

	for (auto _ : state_p)
	{
		FE::block_pool_ptr<std::string> l_smart_ptr = FE::block_pool<std::string>::allocate({"Area B"});
	}

	FE::block_pool<std::string>::shrink_to_fit({ "Area B" });
}
BENCHMARK(FE_block_region_pool);


void FE_generic_pool(benchmark::State& state_p) noexcept
{
	FE::generic_pool<>::create_pages(1);

	for (auto _ : state_p)
	{
		FE::generic_pool_ptr<std::string> l_smart_ptr = FE::generic_pool<>::allocate<std::string>();
	}

	FE::generic_pool<>::shrink_to_fit();
}
BENCHMARK(FE_generic_pool);


//void FE_new_delete_generic_pool_allocator(benchmark::State& state_p) noexcept
//{
//	FE::generic_pool<>::create_pages(1);
//
//	for (auto _ : state_p)
//	{
//		auto l_ptr = FE::new_delete_pool_allocator<std::string>::allocate(1);
//
//		l_ptr = FE::new_delete_pool_allocator<std::string>::reallocate(l_ptr, 1, 2);
//
//		FE::new_delete_pool_allocator<std::string>::deallocate(l_ptr, 2);
//	}
//
//	FE::generic_pool<>::shrink_to_fit();
//}
//BENCHMARK(FE_new_delete_generic_pool_allocator);
//
//
//void FE_std_style_new_delete_generic_pool_allocator(benchmark::State& state_p) noexcept
//{
//	FE::generic_pool<>::create_pages(1);
//	FE::std_style::new_delete_pool_allocator<std::string> l_allocator;
//
//	for (auto _ : state_p)
//	{
//		auto l_ptr = l_allocator.allocate(1);
//
//		l_ptr = l_allocator.reallocate(l_ptr, 1, 2);
//
//		l_allocator.deallocate(l_ptr, 2);
//	}
//
//	FE::generic_pool<>::shrink_to_fit();
//}
//BENCHMARK(FE_std_style_new_delete_generic_pool_allocator);


void boost_block_pool(benchmark::State& state_p) noexcept
{
	boost::fast_pool_allocator<std::string> l_allocator;
	auto l_deleter = [&](std::string* p) { l_allocator.deallocate(p); };
	
	for (auto _ : state_p)
	{
		std::unique_ptr<std::string, decltype(l_deleter)>l_smart_ptr(l_allocator.allocate(), l_deleter);
	}
}
BENCHMARK(boost_block_pool);