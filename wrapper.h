#pragma once
#include <string>
#include <vector>
#include <parsing/astnode.h>
#include <stackdump.h>
#include <value.h>

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
	public:
		struct variable_hit
		{
			::sqf::value value;
			::std::string variable;
			::std::string scopename;
			::std::string type;
			int scope_index;

			variable_hit(int scope_index, ::std::string variable, ::sqf::value::cref value, ::std::string scopename, ::std::string type) :
				scope_index(scope_index), variable(variable), value(value), scopename(scopename), type(type) {}
		};
	private:
		static int active_counter;
		CallbackLogger* m_logger;
		sqf::virtualmachine* m_vm;
	public:
		wrapper(Logger& logger);
		~wrapper();
		std::string preprocess(std::string input, bool& errflag, std::string filename);
		bool is_virtualmachine_running();
		bool is_virtualmachine_done();

		bool parse_sqf(std::string code, std::string path);
		bool parse_config(std::string code, std::string path);
		::sqf::parse::astnode parse_sqf_cst(std::string code, std::string path);
		void add_mapping(std::string virtual_path, std::string physical_path);
		void add_allowed_physical(std::string path);
		std::vector<std::string> get_allowed_physicals();
		std::vector<std::string> get_virtual_paths();

		bool start();
		bool stop();
		bool abort();
		bool assembly_step();
		bool line_step();
		bool leave_scope();
		void set_breakpoint(size_t line, std::string file);
		void remove_breakpoint(size_t line, std::string file);
		std::vector<::sqf::diagnostics::stackdump> get_stackdump();
		std::vector<SqfVm::wrapper::variable_hit> local_variables();
		::sqf::value get_variable(std::string variable_name, std::string ns);
		bool set_variable(std::string variable_name, std::string data, std::string ns);
		::sqf::diagnostics::stackdump get_current_instruction_infos();
		std::pair<std::string, std::string> evaluate(std::string data);
	};
}