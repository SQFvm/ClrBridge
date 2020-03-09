#include "wrapper.h"
#pragma unmanaged
#include <virtualmachine.h>
#include <vmstack.h>
#include <sqfnamespace.h>
#include <commandmap.h>
#include <algorithm>
#include <configdata.h>
#include <convert.h>
#include <instruction.h>

std::atomic<int> SqfVm::wrapper::active_counter = 0;
SqfVm::wrapper::wrapper(Logger& logger)
{
	m_vm = new sqf::virtualmachine(logger);
	if ((++active_counter) == 1)
	{
		sqf::commandmap::get().init();
	}
}

SqfVm::wrapper::~wrapper()
{
	delete m_vm;
	if ((--active_counter) == 0)
	{
		sqf::commandmap::get().uninit();
	}
}

std::string SqfVm::wrapper::preprocess(std::string input, bool& errflag, std::string filename)
{
	return m_vm->preprocess(input, errflag, filename);
}

bool SqfVm::wrapper::is_virtualmachine_running()
{
	return m_vm->status() != ::sqf::virtualmachine::vmstatus::halted &&
		m_vm->status() != ::sqf::virtualmachine::vmstatus::empty &&
		m_vm->status() != ::sqf::virtualmachine::vmstatus::halt_error;
}

bool SqfVm::wrapper::is_virtualmachine_done()
{
	return m_vm->status() == ::sqf::virtualmachine::vmstatus::empty;
}

bool SqfVm::wrapper::parse_sqf(std::string code, std::string path)
{
	return m_vm->parse_sqf(code, path);
}

::sqf::parse::astnode SqfVm::wrapper::parse_sqf_cst(std::string code, std::string path)
{
	return m_vm->parse_sqf_cst(code, path);
}

bool SqfVm::wrapper::parse_config(std::string code, std::string path)
{
	return m_vm->parse_config(code, path);
}

std::shared_ptr<sqf::configdata> SqfVm::wrapper::parse_into_config(std::string code, std::string path)
{
	auto config = std::make_shared<sqf::configdata>();
	if (m_vm->parse_config(code, path, config))
	{
		return config;
	}
	else
	{
		throw std::runtime_error("Parse Failed.");
	}
}

::sqf::parse::astnode SqfVm::wrapper::parse_config_cst(std::string code, std::string path)
{
	return m_vm->parse_config_cst(code, path);
}

void SqfVm::wrapper::add_mapping(std::string virtual_path, std::string physical_path)
{
	m_vm->get_filesystem().add_mapping(virtual_path, physical_path);
}

void SqfVm::wrapper::add_allowed_physical(std::string path)
{
	m_vm->get_filesystem().add_allowed_physical(path);
}

std::vector<std::string> SqfVm::wrapper::get_allowed_physicals()
{
	return m_vm->get_filesystem().m_physicalboundaries;
}
std::vector<std::string> SqfVm::wrapper::get_virtual_paths()
{
	return m_vm->get_filesystem().m_virtualpaths;
}

bool SqfVm::wrapper::start()
{
	return m_vm->execute(sqf::virtualmachine::execaction::start) == sqf::virtualmachine::execresult::OK;
}

bool SqfVm::wrapper::stop()
{
	return m_vm->execute(sqf::virtualmachine::execaction::stop) == sqf::virtualmachine::execresult::OK;
}

bool SqfVm::wrapper::abort()
{
	return m_vm->execute(sqf::virtualmachine::execaction::abort) == sqf::virtualmachine::execresult::OK;
}

bool SqfVm::wrapper::assembly_step()
{
	return m_vm->execute(sqf::virtualmachine::execaction::assembly_step) == sqf::virtualmachine::execresult::OK;
}
bool SqfVm::wrapper::line_step()
{
	return m_vm->execute(sqf::virtualmachine::execaction::line_step) == sqf::virtualmachine::execresult::OK;
}

bool SqfVm::wrapper::leave_scope()
{
	return m_vm->execute(sqf::virtualmachine::execaction::leave_scope) == sqf::virtualmachine::execresult::OK;
}

void SqfVm::wrapper::set_breakpoint(size_t line, std::string file)
{
	sqf::diagnostics::breakpoint bp(line, file);
	m_vm->push_back(bp);
}

void SqfVm::wrapper::remove_breakpoint(size_t line, std::string file)
{
	auto res = std::remove_if(m_vm->breakpoints_begin(), m_vm->breakpoints_end(), [line, file](sqf::diagnostics::breakpoint& breakpoint) -> bool {
		return breakpoint.line() == line && breakpoint.file() == file;
		});
	m_vm->breakpoints_erase(res, m_vm->breakpoints_end());
}

std::vector<::sqf::diagnostics::stackdump> SqfVm::wrapper::get_stackdump()
{
	return m_vm->active_vmstack()->dump_callstack_diff({});
}

::sqf::value SqfVm::wrapper::get_variable(std::string variable_name, std::string ns)
{
	if (ns.empty())
	{
		return m_vm->active_vmstack()->get_variable(variable_name);
	}
	else if (ns == "missionNamespace")
	{
		return m_vm->missionnamespace()->get_variable(variable_name);
	}
	else if (variable_name == "parsingNamespace")
	{
		return m_vm->parsingnamespace()->get_variable(variable_name);
	}
	else if (variable_name == "profileNamespace")
	{
		return m_vm->profilenamespace()->get_variable(variable_name);
	}
	else if (variable_name == "uiNamespace")
	{
		return m_vm->uinamespace()->get_variable(variable_name);
	}
	else
	{
		return {};
	}
}
std::pair<::sqf::value, std::string> SqfVm::wrapper::evaluate(std::string data)
{
	bool success = false;
	auto val = m_vm->evaluate_expression(data, success);
	if (success)
	{
		return { val, sqf::type_str(val.dtype()) };
	}
	return { {}, "" };
}
bool SqfVm::wrapper::set_variable(std::string variable_name, std::string data, std::string ns)
{
	bool success = false;
	auto val = m_vm->evaluate_expression(data, success);
	if (success)
	{
		if (ns.empty())
		{
			m_vm->active_vmstack()->stacks_top()->set_variable(variable_name, val);
		}
		else if (ns == "missionNamespace")
		{
			m_vm->missionnamespace()->set_variable(variable_name, val);
		}
		else if (variable_name == "parsingNamespace")
		{
			m_vm->parsingnamespace()->set_variable(variable_name, val);
		}
		else if (variable_name == "profileNamespace")
		{
			m_vm->profilenamespace()->set_variable(variable_name, val);
		}
		else if (variable_name == "uiNamespace")
		{
			m_vm->uinamespace()->set_variable(variable_name, val);
		}
		else
		{
			success = false;
		}
	}
	return success;
}
::sqf::diagnostics::stackdump SqfVm::wrapper::get_current_instruction_infos()
{
	::sqf::diagnostics::stackdump dump;
	auto current_instruction = m_vm->current_instruction();
	if (current_instruction)
	{
		dump.column = current_instruction->col();
		dump.line = current_instruction->line();
		dump.file = current_instruction->file();
		dump.dbginf = current_instruction->dbginf("DBG");
	}
	return dump;
}
std::vector<SqfVm::wrapper::variable_hit> SqfVm::wrapper::local_variables()
{
	std::vector<variable_hit> variables;
	int i = 0;
	for (auto callstack = m_vm->active_vmstack()->stacks_begin(); callstack != m_vm->active_vmstack()->stacks_end(); ++callstack)
	{
		for (auto& it : callstack->get()->get_variable_map())
		{
			variables.push_back({ i, it.first, it.second, callstack->get()->get_scopename(), ::sqf::type_str(it.second.dtype()) });
		}
		i--;
	}
	return variables;
}
