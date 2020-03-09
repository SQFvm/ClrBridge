#include "Astnode.h"
#include <parsing/astnode.h>


SqfVm::Astnode::Astnode(::sqf::parse::astnode& base)
{
	m_children = gcnew System::Collections::Generic::List<Astnode^>(base.children.size());
	m_offset = base.offset;
	m_length = base.length;
	m_line = base.line;
	m_col = base.col;
	m_file = gcnew System::String(base.file.c_str());
	m_kind = base.kind;
	for (auto& it : base.children)
	{
		m_children->Add(gcnew Astnode(it));
	}
}