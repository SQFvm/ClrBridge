#pragma once
namespace sqf
{
	namespace parse
	{
		class astnode;
	}
}
namespace SqfVm {

	public enum class SqfNodeType
	{
		NA = 0,
		SQF,
		STATEMENT,
		ASSIGNMENT,
		ASSIGNMENTLOCAL,
		BEXP1,
		BEXP2,
		BEXP3,
		BEXP4,
		BEXP5,
		BEXP6,
		BEXP7,
		BEXP8,
		BEXP9,
		BEXP10,
		BINARYEXPRESSION,
		BINARYOP,
		BRACKETS,
		PRIMARYEXPRESSION,
		NULAROP,
		UNARYEXPRESSION,
		UNARYOP,
		NUMBER,
		HEXNUMBER,
		VARIABLE,
		STRING,
		CODE,
		ARRAY
	};
	public ref class SqfNode
	{
	private:
		System::Collections::Generic::List<SqfNode^>^ m_children;
		size_t m_offset;
		size_t m_length;
		size_t m_line;
		size_t m_col;
		short m_kind;
		System::String^ m_file;
	public:
		SqfNode(::sqf::parse::astnode& base);
		System::Collections::Generic::List<SqfNode^>^ GetChildren() { return m_children; }
		size_t GetOffset() { return m_offset; }
		size_t GetLength() { return m_length; }
		size_t GetLine() { return m_line; }
		size_t GetColumn() { return m_col; }
		SqfNodeType GetNodeType() { return static_cast<SqfNodeType>(m_kind); }
		System::String^ GetFile() { return m_file; }

	};
}
