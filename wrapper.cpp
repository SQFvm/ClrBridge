#include "wrapper.h"
#pragma unmanaged
#include <virtualmachine.h>
#include <vmstack.h>
#include <sqfnamespace.h>
#include <commandmap.h>
#include <algorithm>

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
	return m_vm->execute(sqf::virtualmachine::execaction::assembly_step) == sqf::virtualmachine::execresult::OK;
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
	std::remove_if(m_vm->breakpoints_begin(), m_vm->breakpoints_end(), [line, file](sqf::diagnostics::breakpoint& breakpoint) -> bool {
		return breakpoint.line() == line && breakpoint.file() == file;
		});
}

std::vector<::sqf::diagnostics::stackdump> SqfVm::wrapper::get_stackdump()
{
	return m_vm->active_vmstack()->dump_callstack_diff({});
}
