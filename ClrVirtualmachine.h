#pragma once
#include <virtualmachine.h>
#include <commandmap.h>
#include <parsepreprocessor.h>
#include <msclr/marshal_cppstd.h>
#include <stdexcept>
#include <sstream>

using namespace System;

namespace sqfvm {
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
		String^ m_file;
	public:
		SqfNode(astnode& base)
		{
			m_children = gcnew System::Collections::Generic::List<SqfNode^>(base.children.size());
			m_offset = base.offset;
			m_length = base.length;
			m_line = base.line;
			m_col = base.col;
			m_file = gcnew String(base.file.c_str());
			m_kind = base.kind;
			for (auto& it : base.children)
			{
				m_children->Add(gcnew SqfNode(it));
			}
		}
		~SqfNode()
		{

		}
		System::Collections::Generic::List<SqfNode^>^ GetChildren() { return m_children; }
		size_t GetOffset() { return m_offset; }
		size_t GetLength() { return m_length; }
		size_t GetLine() { return m_line; }
		size_t GetColumn() { return m_col; }
		SqfNodeType GetNodeType() { return static_cast<SqfNodeType>(m_kind); }
		String^ GetFile() { return m_file; }
	};
	public ref class ClrVirtualmachine
	{
	private:
		sqf::virtualmachine* m_vm;
		std::stringstream* m_err;
		std::stringstream* m_wrn;
		std::stringstream* m_out;
		static int active_counter = 0;
	public:
		static ClrVirtualmachine()
		{
			sqf::commandmap::get().init();
		}
		ClrVirtualmachine()
		{
			m_vm = new sqf::virtualmachine();
			m_vm->err(m_err);
			m_vm->wrn(m_wrn);
			m_vm->out(m_out);
			if ((++active_counter) == 1)
			{
				sqf::commandmap::get().init();
			}
		}
		~ClrVirtualmachine()
		{
			delete m_vm;
			delete m_err;
			delete m_wrn;
			delete m_out;
			active_counter--;
			if (active_counter == 0)
			{
				sqf::commandmap::get().uninit();
			}
		}

		void ParseSqf(String^ code_m , String^ path_m)
		{
			auto code = msclr::interop::marshal_as<std::string>(code_m);
			auto path = msclr::interop::marshal_as<std::string>(path_m);
			try
			{
				m_vm->parse_sqf(code, path);
			}
			catch (std::exception ex)
			{
				throw gcnew Exception(gcnew String(ex.what()));
			}
		}

		SqfNode^ CreateSqfCst(String^ code_m, String^ path_m)
		{
			auto code = msclr::interop::marshal_as<std::string>(code_m);
			auto path = msclr::interop::marshal_as<std::string>(path_m);
			try
			{
				auto res = m_vm->parse_sqf_cst(code, path);
				return gcnew SqfNode(res);
			}
			catch (std::exception ex)
			{
				throw gcnew Exception(gcnew String(ex.what()));
			}
		}

		String^ PreProcess(String^ code_m, String^ path_m)
		{
			auto code = msclr::interop::marshal_as<std::string>(code_m);
			auto path = msclr::interop::marshal_as<std::string>(path_m);
			bool err = false;
			auto res = sqf::parse::preprocessor::parse(m_vm, code, err, path);
			if (err)
			{
				throw gcnew Exception(gcnew String("PreProcessing failed. See Error Contents for further info."));
			}
			return gcnew String(res.c_str());
		}
		void Execute()
		{
			m_vm->execute();
		}
		String^ ErrorContents()
		{
			auto res = m_err->str();
			m_err->str("");
			return gcnew String(res.c_str());
		}
		String^ WarningContents()
		{
			auto res = m_wrn->str();
			m_wrn->str("");
			return gcnew String(res.c_str());
		}
		String^ InfoContents()
		{
			auto res = m_out->str();
			m_out->str("");
			return gcnew String(res.c_str());
		}
	};
}
