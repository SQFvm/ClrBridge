#pragma once
#include <virtualmachine.h>
#include <vmstack.h>
#include <sqfnamespace.h>
#include <commandmap.h>
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
		SqfNode(::sqf::parse::astnode& base)
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
	public ref class CallstackLayer
	{
	public:
		String^ Namespace;
		size_t Line;
		size_t Column;
		String^ File;
		String^ DebugInformations;
		String^ CallstackName;
		String^ ScopeName;
	};
	public delegate void ClrVirtualmachineLogDelegate(loglevel, String^);
	ref class ClrVirtualmachine;
	class CallbackLogger : public Logger
	{
		// Inherited via Logger
		virtual void log(loglevel level, std::string_view message) override;
	private:
		gcroot<ClrVirtualmachine^> m_clr_virtualmachine;
	public:
		CallbackLogger(ClrVirtualmachine^ clr_virtualmachine) : Logger(), m_clr_virtualmachine(clr_virtualmachine){}
	};
	public ref class ClrVirtualmachine 
	{
	private:
		CallbackLogger* m_logger;
		sqf::virtualmachine* m_vm;
		static int active_counter = 0;
	internal:
		void log(loglevel level, std::string_view message)
		{
			auto str = gcnew String(message.data());
			OnLog(level, str);
		}
	public:
		virtual event ClrVirtualmachineLogDelegate^ OnLog;

		ClrVirtualmachine()
		{
			m_logger = new CallbackLogger(this);
			m_vm = new sqf::virtualmachine(*m_logger);
			if ((++active_counter) == 1)
			{
				sqf::commandmap::get().init();
			}
		}
		~ClrVirtualmachine()
		{
			delete m_vm;
			delete m_logger;
			active_counter--;
			if (active_counter == 0)
			{
				sqf::commandmap::get().uninit();
			}
		}
		property bool IsVirtualmachineRunning
		{
			bool get()
			{
				return m_vm->status() != ::sqf::virtualmachine::vmstatus::halted &&
					m_vm->status() != ::sqf::virtualmachine::vmstatus::empty &&
					m_vm->status() != ::sqf::virtualmachine::vmstatus::halt_error;
			}
		}

		void AddMapping(String^ virtual_path_m, String^ physical_path_m)
		{
			auto physical_path = msclr::interop::marshal_as<std::string>(physical_path_m);
			auto virtual_path = msclr::interop::marshal_as<std::string>(virtual_path_m);
			this->m_vm->get_filesystem().add_mapping(virtual_path, physical_path);
		}
		void AddAllowedPath(String^ path_m)
		{
			auto path = msclr::interop::marshal_as<std::string>(path_m);
			this->m_vm->get_filesystem().add_allowed_physical(path);
		}
		String^ PreProcess(String^ code_m, String^ path_m)
		{
			auto code = msclr::interop::marshal_as<std::string>(code_m);
			auto path = msclr::interop::marshal_as<std::string>(path_m);
			try
			{
				bool errflag = false;
				std::string res = m_vm->preprocess(code, errflag, path);
				if (errflag)
				{
					throw gcnew Exception(gcnew String("PreProcessing failed. See Error Contents for further info."));
				}
				else
				{
					return gcnew String(res.c_str());
				}
			}
			catch (std::exception ex)
			{
				throw gcnew Exception(gcnew String(ex.what()));
			}
		}
		void ParseSqf(String^ code_m, String^ path_m)
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
				sqf::parse::astnode res = m_vm->parse_sqf_cst(code, path);
				return gcnew SqfNode(res);
			}
			catch (std::exception ex)
			{
				throw gcnew Exception(gcnew String(ex.what()));
			}
		}
		bool Start()
		{
			return m_vm->execute(sqf::virtualmachine::execaction::start) == sqf::virtualmachine::execresult::OK;
		}
		bool Stop()
		{
			return m_vm->execute(sqf::virtualmachine::execaction::stop) == sqf::virtualmachine::execresult::OK;
		}
		bool Abort()
		{
			return m_vm->execute(sqf::virtualmachine::execaction::abort) == sqf::virtualmachine::execresult::OK;
		}
		bool AssemblyStep()
		{
			return m_vm->execute(sqf::virtualmachine::execaction::abort) == sqf::virtualmachine::execresult::OK;
		}
		bool LeaveScope()
		{
			return m_vm->execute(sqf::virtualmachine::execaction::abort) == sqf::virtualmachine::execresult::OK;
		}
		System::Collections::Generic::List<CallstackLayer^>^ GetCallstack()
		{
			auto res = m_vm->active_vmstack()->dump_callstack_diff({});
			auto list = gcnew System::Collections::Generic::List<CallstackLayer^>(res.size());
			for each (auto it in res)
			{
				auto layer = gcnew CallstackLayer();
				layer->CallstackName = gcnew String(it.callstack_name.c_str());
				layer->Column = it.column;
				layer->DebugInformations = gcnew String(it.dbginf.c_str());
				layer->File = gcnew String(it.file.c_str());
				layer->ScopeName = gcnew String(it.scope_name.c_str());
				layer->Namespace = gcnew String(it.namespace_used->get_name().c_str());
				layer->Line = it.line;
				list->Add(layer);
			}
			return list;
		}
};
}
