#pragma once
#include <msclr/marshal_cppstd.h>
#include <stdexcept>
#include <sstream>


#include <logging.h>
#include "wrapper.h"
#include "stackdump.h"
#include "sqfnamespace.h"

#include "SqfNode.h"
#include "CallstackLayer.h"

using namespace System;

namespace SqfVm {
	public enum class ESeverity
	{
		Fatal,
		Error,
		Warning,
		Info,
		Verbose,
		Trace
	};
	public ref struct LogEventArgs
	{
	public:
		property ESeverity Severity;
		property String^ Message;
	};
	public delegate void ClrVirtualmachineLogDelegate(Object^ sender, LogEventArgs^ eventArgs);
	ref class ClrVirtualmachine;
	class CallbackLogger : public Logger
	{
		// Inherited via Logger
		virtual void log(loglevel level, std::string_view message) override;
	private:
		gcroot<ClrVirtualmachine^> m_clr_virtualmachine;
	public:
		CallbackLogger(ClrVirtualmachine^ clr_virtualmachine) : Logger(), m_clr_virtualmachine(clr_virtualmachine) {}
	};

	public ref class ClrVirtualmachine 
	{
	private:
		CallbackLogger* m_logger;
		wrapper* m_wrapper;
	internal:
		void log(loglevel level, std::string_view message)
		{
			LogEventArgs^ eventArgs = gcnew LogEventArgs();
			eventArgs->Message = gcnew String(message.data());
			switch (level)
			{
			case loglevel::fatal:
				eventArgs->Severity = ESeverity::Fatal;
				break;
			case loglevel::error:
				eventArgs->Severity = ESeverity::Error;
				break;
			case loglevel::warning:
				eventArgs->Severity = ESeverity::Warning;
				break;
			case loglevel::info:
				eventArgs->Severity = ESeverity::Info;
				break;
			case loglevel::verbose:
				eventArgs->Severity = ESeverity::Verbose;
				break;
			case loglevel::trace:
				eventArgs->Severity = ESeverity::Trace;
				break;
			default:
				break;
			}
			OnLog(this, eventArgs);
		}
	public:
		virtual event ClrVirtualmachineLogDelegate^ OnLog;

		ClrVirtualmachine()
		{
			m_logger = new CallbackLogger(this);
			m_wrapper = new wrapper(*m_logger);
		}
		~ClrVirtualmachine()
		{
			delete m_wrapper;
			delete m_logger;
		}
		property bool IsVirtualmachineRunning
		{
			bool get()
			{
				return m_wrapper->is_virtualmachine_running();
			}
		}
		property bool IsVirtualmachineDone
		{
			bool get()
			{
				return m_wrapper->is_virtualmachine_done();
			}
		}

		void AddMapping(String^ virtual_path_m, String^ physical_path_m)
		{
			auto physical_path = msclr::interop::marshal_as<std::string>(physical_path_m);
			auto virtual_path = msclr::interop::marshal_as<std::string>(virtual_path_m);
			m_wrapper->add_mapping(virtual_path, physical_path);
		}
		void AddAllowedPath(String^ path_m)
		{
			auto path = msclr::interop::marshal_as<std::string>(path_m);
			m_wrapper->add_allowed_physical(path);
		}
		String^ PreProcess(String^ code_m, String^ path_m)
		{
			auto code = msclr::interop::marshal_as<std::string>(code_m);
			auto path = msclr::interop::marshal_as<std::string>(path_m);
			try
			{
				bool errflag = false;
				std::string res = m_wrapper->preprocess(code, errflag, path);
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
				m_wrapper->parse_sqf(code, path);
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
				sqf::parse::astnode res = m_wrapper->parse_sqf_cst(code, path);
				return gcnew SqfNode(res);
			}
			catch (std::exception ex)
			{
				throw gcnew Exception(gcnew String(ex.what()));
			}
		}
		bool Start()
		{
			return m_wrapper->start();
		}
		bool Stop()
		{
			return m_wrapper->stop();
		}
		bool Abort()
		{
			return m_wrapper->abort();
		}
		bool AssemblyStep()
		{
			return m_wrapper->assembly_step();
		}
		bool LeaveScope()
		{
			return m_wrapper->leave_scope();
		}
		void SetBreakpoint(int line, String^ s_file)
		{
			auto file = msclr::interop::marshal_as<std::string>(s_file);
			m_wrapper->set_breakpoint(line, file);
		}
		void RemoveBreakpoint(int line, String^ s_file)
		{
			auto file = msclr::interop::marshal_as<std::string>(s_file);
			m_wrapper->remove_breakpoint(line, file);
		}
		System::Collections::Generic::List<CallstackLayer^>^ GetCallstack()
		{
			auto res = m_wrapper->get_stackdump();
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
