#include "stdafx.h"
#include "ClrVirtualmachine.h"

void sqfvm::CallbackLogger::log(loglevel level, std::string_view message)
{
	m_clr_virtualmachine->log(level, message);
}
