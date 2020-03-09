#include "Config.h"
#include "ClrVirtualmachine.h"
#include <configdata.h>

int SqfVm::Config::Count::get()
{
	return (int)(*m_configdata)->size();
}
SqfVm::EConfigNodeType SqfVm::Config::NodeType::get()
{
	if ((*m_configdata)->is_null())
	{
		throw gcnew System::InvalidOperationException();
	}
	else if ((*m_configdata)->cfgvalue().dtype() == sqf::type::STRING)
	{
		return SqfVm::EConfigNodeType::String;
	}
	else if ((*m_configdata)->cfgvalue().dtype() == sqf::type::SCALAR)
	{
		return SqfVm::EConfigNodeType::Scalar;
	}
	else if ((*m_configdata)->cfgvalue().dtype() == sqf::type::NOTHING)
	{
		return SqfVm::EConfigNodeType::Config;
	}
	else if ((*m_configdata)->cfgvalue().dtype() == sqf::type::ARRAY)
	{
		return SqfVm::EConfigNodeType::Array;
	}
	else
	{
		throw gcnew System::InvalidOperationException();
	}
}
System::Object^ SqfVm::Config::Value::get()
{
	auto res = (*m_configdata)->cfgvalue();
	return SqfVm::ClrVirtualmachine::FromValue(res);
}
SqfVm::Config^ SqfVm::Config::default::get(int index)
{
	auto res = (*m_configdata)->at((size_t)index);
	if (res.dtype() == sqf::type::CONFIG)
	{
		return gcnew SqfVm::Config(res.data<sqf::configdata>());
	}
	else
	{
		return nullptr;
	}
}
SqfVm::Config^ SqfVm::Config::default::get(System::String^ index)
{
	auto key = msclr::interop::marshal_as<std::string>(index);
	auto res = (*m_configdata)->navigate(key);
	if (res.dtype() == sqf::type::CONFIG)
	{
		return gcnew SqfVm::Config(res.data<sqf::configdata>());
	}
	else
	{
		return nullptr;
	}
}