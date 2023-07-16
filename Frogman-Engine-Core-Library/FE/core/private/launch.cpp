﻿// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include "launch.hpp"
#include <FE/core/fstream_guard.hxx>
#include <boost/stacktrace/stacktrace.hpp>
#include <csignal>


// read serialized configuration file. 
void ::FE::internal::engine_main::initialize_engine(_MAYBE_UNUSED_ engine_main_initialization_arguments engine_main_initialization_arguments_p) noexcept
{
	static ::FE::var::boolean l_s_is_initialized = false;
	ABORT_IF(l_s_is_initialized == true, "ERROR: The initialization cannot be done more than once");
	l_s_is_initialized = true;

#ifdef _IS_EXCEPTION_LOGGER_ENABLED_
	ABORT_IF(engine_main_initialization_arguments_p._exception_initialization_arguments._exception_logging_strategy_ptr == nullptr, "CRITICAL ERROR: engine_main_initialization_arguments_p._exception_initialization_arguments._exception_logging_strategy_ptr cannot be nullptr");
	ABORT_IF(engine_main_initialization_arguments_p._exception_initialization_arguments._exception_log_buffer_size == 0, "CRITICAL ERROR: engine_main_initialization_arguments_p._exception_initialization_arguments._exception_log_buffer_size cannot be ZERO");
	::FE::exception::s_logging_strategy_ptr = engine_main_initialization_arguments_p._exception_initialization_arguments._exception_logging_strategy_ptr;
	
	::FE::exception::s_log_buffer_size = std::move(engine_main_initialization_arguments_p._exception_initialization_arguments._exception_log_buffer_size);
	::FE::exception::s_write_operation_triggering_point = std::move(engine_main_initialization_arguments_p._exception_initialization_arguments._write_operation_triggering_point);

	::FE::exception::__construct_exception_on_main_thread();
#endif
	
	::std::signal(SIGTERM, abnormal_shutdown_with_exit_code);
	::std::signal(SIGSEGV, abnormal_shutdown_with_exit_code);
	::std::signal(SIGILL, abnormal_shutdown_with_exit_code);
	::std::signal(SIGABRT, abnormal_shutdown_with_exit_code);
	::std::signal(SIGFPE, abnormal_shutdown_with_exit_code);
}

void ::FE::internal::engine_main::shutdown_engine() noexcept
{
#ifdef _IS_EXCEPTION_LOGGER_ENABLED_
	::FE::exception::__destruct_exception_on_main_thread();
#endif
}

_NORETURN_ void ::FE::internal::engine_main::abnormal_shutdown_with_exit_code(int32 signal_p) noexcept
{
	boost::stacktrace::stacktrace l_stack_frame_dumps;
	var::length_t l_stack_depth = 0;

#ifdef _IS_EXCEPTION_LOGGER_ENABLED_
	exception::tl_s_file_logger << "\n-------------------------------------------------- BEGIN STACK TRACE RECORD --------------------------------------------------\n\n";

	auto l_crend = l_stack_frame_dumps.crend();
	for (auto crbegin = l_stack_frame_dumps.crbegin(); crbegin != l_crend; ++crbegin)
	{
		exception::tl_s_file_logger << "#" << l_stack_depth << "\t";
		exception::tl_s_file_logger << boost::stacktrace::to_string(*crbegin).data();
		exception::tl_s_file_logger << "\n\n";
		++l_stack_depth;
	}

	exception::tl_s_file_logger << "-------------------------------------------------- END OF STACK TRACE RECORD --------------------------------------------------\n";
	
	::FE::exception::__destruct_exception();
	::std::exit(signal_p);

#else
	std::ofstream l_release_build_crash_report;
	{
		FE::ofstream_guard l_release_build_crash_report_guard(l_release_build_crash_report, "Crashed Thread Stack Frame Dump Report.txt");

		l_release_build_crash_report << "\n-------------------------------------------------- BEGIN STACK TRACE RECORD --------------------------------------------------\n\n";

		auto l_crend = l_stack_frame_dumps.crend();
		for (auto crbegin = l_stack_frame_dumps.crbegin(); crbegin != l_crend; ++crbegin)
		{
			l_release_build_crash_report << "#" << l_stack_depth << "\t";
			l_release_build_crash_report << boost::stacktrace::to_string(*crbegin).data();
			l_release_build_crash_report << "\n\n";
			++l_stack_depth;
		}

		l_release_build_crash_report << "-------------------------------------------------- END OF STACK TRACE RECORD --------------------------------------------------\n";

	}
	raise(signal_p);
#endif
}
