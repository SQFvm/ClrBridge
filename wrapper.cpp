#include "wrapper.h"
extern "C++"
{
	#include <virtualmachine.h>
	#include <vmstack.h>
	#include <sqfnamespace.h>
	#include <commandmap.h>
}

int SqfVm::wrapper::active_counter = 0;
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
	active_counter--;
	if (active_counter == 0)
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

bool SqfVm::wrapper::parse_sqf(std::string code, std::string path)
{
	return m_vm->parse_sqf(code, path);
}

::sqf::parse::astnode SqfVm::wrapper::parse_sqf_cst(std::string code, std::string path)
{
	return m_vm->parse_sqf_cst(code, path);
}

void SqfVm::wrapper::add_mapping(std::string virtual_path, std::string physical_path)
{
	m_vm->get_filesystem().add_mapping(virtual_path, physical_path);
}

void SqfVm::wrapper::add_allowed_physical(std::string path)
{
	m_vm->get_filesystem().add_allowed_physical(path);
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
	return m_vm->execute(sqf::virtualmachine::execaction::abort) == sqf::virtualmachine::execresult::OK;
}

bool SqfVm::wrapper::leave_scope()
{
	return m_vm->execute(sqf::virtualmachine::execaction::abort) == sqf::virtualmachine::execresult::OK;
}

std::vector<::sqf::diagnostics::stackdump> SqfVm::wrapper::get_stackdump()
{
	return m_vm->active_vmstack()->dump_callstack_diff({});
}
