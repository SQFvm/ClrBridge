#pragma once
#include <memory>
using namespace System::Collections::Generic;

namespace sqf
{
	class configdata;
}
namespace SqfVm {
	public enum class EConfigNodeType
	{
		Config,
		Scalar,
		String,
		Array
	};
	public ref class Config : public IEnumerable<Config^>, public IReadOnlyDictionary<System::String^, Config^>
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
			ref class ConfigDictionaryEnumerator : public IEnumerator<System::Collections::Generic::KeyValuePair<System::String^, SqfVm::Config^>> {
			private:
				Config^ m_config;
				int m_current_index;
			public:
				ConfigDictionaryEnumerator(Config^ config) : m_config(config), m_current_index(-1) {}
				virtual ~ConfigDictionaryEnumerator() { }
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
				virtual property System::Collections::Generic::KeyValuePair<System::String^, SqfVm::Config^> Current
				{
					System::Collections::Generic::KeyValuePair<System::String^, SqfVm::Config^> get()
					{
						if (m_current_index >= 0 && m_current_index <= m_config->Count)
						{
							return System::Collections::Generic::KeyValuePair<System::String^, SqfVm::Config^>(m_config[m_current_index]->Name, m_config[m_current_index]);
						}
						else
						{
							throw gcnew System::InvalidOperationException();
						}
					}
				}
			};
	internal:
			Config(const std::shared_ptr<sqf::configdata>& configdata) :
				m_configdata(new std::shared_ptr<sqf::configdata>(configdata))
			{
			}
			~Config()
			{
				delete m_configdata;
			}
	public:
		void MergeWith(Config^ otherconfig);
			property System::String^ Name { System::String^ get(); }
			property EConfigNodeType NodeType { EConfigNodeType get(); }
			property System::Object^ Value { System::Object^ get(); }
			virtual property int Count { int get(); }
			virtual property System::String^ ParentName { System::String^ get(); }
			property Config^ default[int]{ Config^ get(int index); }
			virtual property Config^ default[System::String^]{ Config^ get(System::String^ index); }

			virtual property Config^ LogicalParent { Config^ get(); }
			virtual property Config^ InheritedParent { Config^ get(); }
			// Inherited via IEnumerable
			virtual System::Collections::IEnumerator^ EnumerableGetEnumerator() = System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}
			// Inherited via IReadOnlyCollection
			virtual System::Collections::Generic::IEnumerator<System::Collections::Generic::KeyValuePair<System::String^, SqfVm::Config^>>^ DictionaryGetEnumerator() = IEnumerable<System::Collections::Generic::KeyValuePair<System::String^, SqfVm::Config^>>::GetEnumerator
			{
				return gcnew ConfigDictionaryEnumerator(this);
			}

			virtual System::Collections::Generic::IEnumerator<SqfVm::Config^>^ GetEnumerator()
			{
				return gcnew ConfigEnumerator(this);
			}




			// Inherited via IReadOnlyDictionary
			virtual property System::Collections::Generic::IEnumerable<System::String^>^ Keys { System::Collections::Generic::IEnumerable<System::String^>^ get(); }

			virtual property System::Collections::Generic::IEnumerable<SqfVm::Config^>^ Values { System::Collections::Generic::IEnumerable<SqfVm::Config^>^ get(); }

			virtual bool ContainsKey(System::String^ key);

			virtual bool TryGetValue(System::String^ key, SqfVm::Config^% value);

			virtual System::String^ ToString() override
			{
				return System::String::Concat(gcnew System::String("Config (Name: "), Name, gcnew System::String(", NodeType: "), NodeType, gcnew System::String(")"));
			}

};
}