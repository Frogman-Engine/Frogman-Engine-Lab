// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <FE/core/prerequisites.h>
#include <FE/framework/framework.hpp>
#include <FE/miscellaneous/private/macro_restrictions.h>




//https://learn.microsoft.com/en-us/cpp/build/profile-guided-optimizations?view=msvc-170
// Profile Guided Optimization
class test final : public FE::framework::application
{
	virtual void set_up(_MAYBE_UNUSED_ int argc_p, _MAYBE_UNUSED_ char** argv_p) override final
	{
		testing::InitGoogleTest(&argc_p, argv_p);

		if (argv_p == nullptr)
		{
			char arg0_default[] = "benchmark";
			char* args_default = arg0_default;

			argc_p = 1;
			argv_p = &args_default;
		}
		benchmark::Initialize(&argc_p, argv_p);
		FE_ASSERT(benchmark::ReportUnrecognizedArguments(argc_p, argv_p) == true, "Assertion Failed: Unrecognized Benchmark Arguments Detected.");
	}
	
	virtual int run(_MAYBE_UNUSED_ int argc_p, _MAYBE_UNUSED_ char** argv_p) override final
	{
		let int32 l_exit_code = RUN_ALL_TESTS();
		std::cerr << "\n\n";
		benchmark::RunSpecifiedBenchmarks();
		std::cerr << "\n\n";
		return l_exit_code;
	}

	virtual void clean_up() override final
	{
		benchmark::Shutdown();
	}
};




FE::framework::discard_t g_discard = FE::framework::application::set_application
(
	[]()
	{
		static test l_s_test_application;

		FE::framework::initialization_arguments l_configs;
		l_configs._initial_function_table_size = 1000;
		l_configs._application_pointer = &l_s_test_application;
		return l_configs;
	}()
);