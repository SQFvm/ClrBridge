#include "Config.h"
#include "ClrVirtualmachine.h"
#include <configdata.h>

int SqfVm::Config::Count::get()
{
	return (int)(*m_configdata)->size();
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