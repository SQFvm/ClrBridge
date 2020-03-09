#include "Config.h"
#include "ClrVirtualmachine.h"
#include <configdata.h>

int SqfVm::Config::Count::get()
{
	return (int)(*m_configdata)->size();
}
System::String^ SqfVm::Config::Name::get()
{
	return gcnew System::String((*m_configdata)->name().c_str());
}
System::String^ SqfVm::Config::ParentName::get()
{
	return gcnew System::String((*m_configdata)->inherited_parent_name().c_str());
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
SqfVm::Config^ SqfVm::Config::LogicalParent::get()
{
	auto res = (*m_configdata)->logical_parent();
	if (res.dtype() == sqf::type::CONFIG)
	{
		return gcnew Config(res.data<sqf::configdata>());
	}
	else
	{
		return nullptr;
	}
}
SqfVm::Config^ SqfVm::Config::InheritedParent::get()
{
	auto res = (*m_configdata)->inherited_parent();
	if (res.dtype() == sqf::type::CONFIG)
	{
		return gcnew Config(res.data<sqf::configdata>());
	}
	else
	{
		return nullptr;
	}
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
	if (res.dtype() == sqf::type::CONFIG && !res.data<sqf::configdata>()->is_null())
	{
		return gcnew SqfVm::Config(res.data<sqf::configdata>());
	}
	else
	{
		return nullptr;
	}
}

System::Collections::Generic::IEnumerable<System::String^>^ SqfVm::Config::Keys::get()
{
	auto list = gcnew System::Collections::Generic::List<System::String^>((*m_configdata)->size());
	for (int i = 0; i < (int)(*m_configdata)->size(); i++)
	{
		auto val = (*m_configdata)->at(i);
		if (val.dtype() == sqf::type::CONFIG)
		{
			list->Add(gcnew System::String(val.data<sqf::configdata>()->name().c_str()));
		}
		else
		{
			// Unknown Cause.
			throw gcnew System::Exception();
		}
	}
	return list;
}
System::Collections::Generic::IEnumerable<SqfVm::Config^>^ SqfVm::Config::Values::get()
{
	auto list = gcnew System::Collections::Generic::List<SqfVm::Config^>((*m_configdata)->size());
	for (int i = 0; i < (int)(*m_configdata)->size(); i++)
	{
		list->Add(this[i]);
	}
	return list;
}

void SqfVm::Config::MergeWith(Config^ otherconfig)
{
	if (otherconfig)
	{
		(*otherconfig->m_configdata)->mergeinto(*m_configdata);
	}
	else
	{
		throw gcnew System::ArgumentNullException(gcnew String("otherconfig"));
	}
}

bool SqfVm::Config::ContainsKey(System::String^ key)
{
	for (int i = 0; i < (int)(*m_configdata)->size(); i++)
	{
		auto val = (*m_configdata)->at(i);
		if (val.dtype() == sqf::type::CONFIG)
		{
			return true;
		}
	}
	return false;
}

bool SqfVm::Config::TryGetValue(System::String^ key, SqfVm::Config^% value)
{
	for (int i = 0; i < (int)(*m_configdata)->size(); i++)
	{
		auto val = (*m_configdata)->at(i);
		if (val.dtype() == sqf::type::CONFIG)
		{
			value = gcnew SqfVm::Config(val.data<sqf::configdata>());
			return true;
		}
	}
	value = nullptr;
	return false;
}
