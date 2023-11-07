#ifndef _FE_FRAMEWORK_HPP_
#define _FE_FRAMEWORK_HPP_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>




int main(int argc_p, char** argv_p);




BEGIN_NAMESPACE(FE::framework)


using discard_t = bool;

struct initialization_arguments
{
	using initial_function_table_size_t = var::size_t;

	initial_function_table_size_t _initial_function_table_size;
	class application* _application_pointer;
};


enum struct RESTART_OR_NOT : uint8
{
	_NO_OPERATION = 0,
	_HAS_TO_RESTART = 1,
};


class application
{
	friend int ::main(int argc_p, char** argv_p);
	static initialization_arguments s_options;

protected:
	virtual void set_up(int argc_p, char** argv_p) = 0;
	virtual int run(int argc_p, char** argv_p) = 0;
	virtual void clean_up() = 0;

public:
	static discard_t set_application(initialization_arguments initialization_arguments_p) noexcept;
	static RESTART_OR_NOT s_restart_or_not;

private:
	static initialization_arguments get_application() noexcept;

private:
	static void __launch_main(initialization_arguments& initialization_arguments_p);
	static void __shutdown_main();
	_NORETURN_ static void __abnormal_shutdown_with_exit_code(int32 signal_p);
};


struct restart_request {};


END_NAMESPACE
#endif