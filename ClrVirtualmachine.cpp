#include "ClrVirtualmachine.h"
#include <arraydata.h>

void SqfVm::CallbackLogger::log(loglevel level, std::string_view message)
{
	m_clr_virtualmachine->log(level, message);
}

System::Object^ SqfVm::ClrVirtualmachine::FromValue(sqf::value::cref val)
{
	switch (val.dtype())
	{
	case sqf::type::BOOL:
		return gcnew System::Boolean(val.as_bool());
	case sqf::type::SCALAR:
		return gcnew System::Double(val.as_double());
	case sqf::type::TEXT:
	case sqf::type::STRING:
	{
		auto str = val.as_string();
		return gcnew System::String(str.c_str());
	}
	case sqf::type::ARRAY:
	{
		auto arrdata = val.data<sqf::arraydata>();
		auto list = gcnew System::Collections::ArrayList(arrdata->size());
		for (int i = 0; i < (int)arrdata->size(); i++)
		{
			list->Add(FromValue(arrdata->at(i)));
		}
		return list;
	}
	default:
	{
		auto str = val.tosqf();
		return gcnew System::String(str.c_str());
	}
	}
}
