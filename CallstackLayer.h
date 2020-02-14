#pragma once
#include <msclr/marshal_cppstd.h>
using namespace System;

public ref class CallstackLayer
{
public:
	property String^ Namespace;
	property size_t Line;
	property size_t Column;
	property String^ File;
	property String^ DebugInformations;
	property String^ CallstackName;
	property String^ ScopeName;

	CallstackLayer();
};

