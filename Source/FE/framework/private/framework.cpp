// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/framework/framework.hpp>
#include <FE/framework/reflection/function_table.hpp>
#include <FE/framework/private/internal_functions.h>
#include <FE/core/fstream_guard.hxx>
#include <FE/core/fstring.hxx>
#include <boost/stacktrace.hpp>
#include <csignal>


#define _DUMP_FILE_NAME_LENGTH_ 128




BEGIN_NAMESPACE(FE::framework)


initialization_arguments application::s_options;

discard_t application::set_application(initialization_arguments initialization_arguments_p) noexcept
{
	FE_CHECK(initialization_arguments_p._application_pointer == nullptr, "ERROR: initialization_arguments_p._application_pointer cannot be nullptr");
	s_options._application_pointer = initialization_arguments_p._application_pointer;
	s_options._initial_function_table_size = initialization_arguments_p._initial_function_table_size;
	return true;
}

initialization_arguments application::get_application() noexcept
{
	return application::s_options;
}


void application::__launch_main(initialization_arguments& initialization_arguments_p)
{
	std::signal(SIGTERM, __abnormal_shutdown_with_exit_code);
	std::signal(SIGSEGV, __abnormal_shutdown_with_exit_code);
	std::signal(SIGILL, __abnormal_shutdown_with_exit_code);
	std::signal(SIGABRT, __abnormal_shutdown_with_exit_code);
	std::signal(SIGFPE, __abnormal_shutdown_with_exit_code);
	std::set_terminate([]() { __abnormal_shutdown_with_exit_code(SIGTERM); });


	function_table::tl_s_task_map.reserve(initialization_arguments_p._initial_function_table_size);
}

void application::__shutdown_main()
{
}

_NORETURN_ void application::__abnormal_shutdown_with_exit_code(int32 signal_p)
{
#ifdef _RELEASE_
	boost::stacktrace::stacktrace l_stack_trace_dumps;

	std::ofstream l_release_build_crash_report;
	{
		FE::fstring<_DUMP_FILE_NAME_LENGTH_> l_dump_filename = "Crashed Thread Stack Trace Report from ";
		l_dump_filename += internal::get_current_local_time();
		l_dump_filename += ".txt";
		FE::ofstream_guard l_release_build_crash_report_guard(l_release_build_crash_report, l_dump_filename.c_str());
		l_release_build_crash_report << "Compilation Date: " << " " << __DATE__ << " - " << __TIME__ << "\n\n";
		l_release_build_crash_report << "\n-------------------------------------------------- BEGIN STACK TRACE RECORD --------------------------------------------------\n\n";

		l_release_build_crash_report << boost::stacktrace::to_string(l_stack_trace_dumps).data() << '\n';

		l_release_build_crash_report << "\n-------------------------------------------------- END OF STACK TRACE RECORD --------------------------------------------------\n";

	}
#endif

	FE::framework::application::s_options._application_pointer->clean_up();
	FE::framework::application::__shutdown_main();

	std::exit(signal_p);
}


END_NAMESPACE




FE::framework::RESTART_OR_NOT FE::framework::application::s_restart_or_not = FE::framework::RESTART_OR_NOT::_NO_OPERATION;


int main(int argc_p, char** argv_p)
{
	int l_exit_code;

	do
	{
		FE::framework::application::s_restart_or_not = FE::framework::RESTART_OR_NOT::_NO_OPERATION;

		auto l_config_args = FE::framework::application::get_application();
		FE_CHECK(FE::framework::application::s_options._application_pointer == nullptr, "Assertion Failure: ${%s@0} is nullptr.", TO_STRING(FE::framework::application::s_options._application_pointer));
		FE::framework::application::__launch_main(l_config_args);

		FE::framework::application::s_options._application_pointer->set_up(argc_p, argv_p);
		l_exit_code = FE::framework::application::s_options._application_pointer->run(argc_p, argv_p);
		FE::framework::application::s_options._application_pointer->clean_up();

		FE::framework::application::__shutdown_main();
	}
	while (FE::framework::application::s_restart_or_not == FE::framework::RESTART_OR_NOT::_HAS_TO_RESTART);

	return l_exit_code;
}