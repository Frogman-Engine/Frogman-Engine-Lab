// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/heap_memory.hpp>
#include <FE/core/clock.hpp>


std::atomic_int64_t FE::heap_memory_tracker_base::s_global_total_bytes = 0;

thread_local FE::var::int64 FE::heap_memory_tracker_base::tl_s_thread_local_total_bytes = 0;