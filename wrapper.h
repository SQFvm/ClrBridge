#pragma once
#include <string>
#include <vector>
#include <parsing/astnode.h>
#include <stackdump.h>

class Logger;
namespace sqf
{
	class virtualmachine;
}

namespace SqfVm
{
	class CallbackLogger;
	class wrapper
	{
		static int active_counter;
		CallbackLogger* m_logger;
		sqf::virtualmachine* m_vm;
	public:
		wrapper(Logger& logger);
		~wrapper();
		std::string preprocess(std::string input, bool& errflag, std::string filename);
		bool is_virtualmachine_running();

		bool parse_sqf(std::string code, std::string path);
		::sqf::parse::astnode parse_sqf_cst(std::string code, std::string path);
		void add_mapping(std::string virtual_path, std::string physical_path);
		void add_allowed_physical(std::string path);

		bool start();
		bool stop();
		bool abort();
		bool assembly_step();
		bool leave_scope();
		std::vector<::sqf::diagnostics::stackdump> get_stackdump();
	};
}