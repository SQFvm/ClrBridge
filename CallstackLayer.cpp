#include "CallstackLayer.h"


CallstackLayer::CallstackLayer()
{
	Namespace = nullptr;
	Line = 0;
	Column = 0;
	File = nullptr;
	DebugInformations = nullptr;
	CallstackName = nullptr;
	ScopeName = nullptr;
}
