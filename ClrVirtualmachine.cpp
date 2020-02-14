#include "ClrVirtualmachine.h"

void SqfVm::CallbackLogger::log(loglevel level, std::string_view message)
{
	m_clr_virtualmachine->log(level, message);
}
