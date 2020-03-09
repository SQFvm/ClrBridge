#pragma once
#include <memory>
using namespace System::Collections::Generic;

namespace sqf
{
	class configdata;
}
namespace SqfVm {
	public ref class Config : public IEnumerable<Config^>
	{
		private:
			std::shared_ptr<sqf::configdata>* m_configdata;
			ref class ConfigEnumerator : public IEnumerator<Config^> {
			private:
				Config^ m_config;
				int m_current_index;
			public:
				ConfigEnumerator(Config^ config) : m_config(config), m_current_index(-1) {}
				virtual ~ConfigEnumerator() { }
				virtual bool EnumeratorMoveNext() = System::Collections::IEnumerator::MoveNext{ return MoveNext(); }
				virtual bool MoveNext()
				{
					m_current_index++;
					return m_config->Count > m_current_index;
				}
				virtual void EnumeratorReset() = System::Collections::IEnumerator::Reset{ Reset(); }
				virtual void Reset()
				{
					m_current_index = -1;
				}
				virtual property System::Object^ EnumeratorCurrent { System::Object^ get() sealed = System::Collections::IEnumerator::Current::get{ return Current; } }
				virtual property SqfVm::Config^ Current
				{
					SqfVm::Config^ get()
					{
						if (m_current_index >= 0 && m_current_index <= m_config->Count)
						{
							return m_config[m_current_index];
						}
						else
						{
							throw gcnew System::InvalidOperationException();
						}
					}
				}
			};
		public:
			Config(const std::shared_ptr<sqf::configdata>& configdata) :
				m_configdata(new std::shared_ptr<sqf::configdata>(configdata))
			{
			}
			~Config()
			{
				delete m_configdata;
			}

			property System::Object^ Value { System::Object^ get(); }
			property int Count { int get(); }
			property Config^ default[int]{ Config^ get(int index); }
			property Config^ default[System::String^]{ Config ^ get(System::String^ index); }

			// Inherited via IEnumerable
			virtual System::Collections::IEnumerator^ EnumerableGetEnumerator() = System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}

			virtual System::Collections::Generic::IEnumerator<SqfVm::Config^>^ GetEnumerator()
			{
				return gcnew ConfigEnumerator(this);
			}

	};
}