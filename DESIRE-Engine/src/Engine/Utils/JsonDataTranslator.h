#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/String/DynamicString.h"

#include "rapidjson/include/rapidjson/document.h"

// --------------------------------------------------------------------------------------------------------------------
//	JsonDataTranslator is a utility class template that can fill a data structure from a JSON object.
//	It stores an array of the value names and their pointer-to-member that we want to translate.
// --------------------------------------------------------------------------------------------------------------------

template<typename T>
class JsonDataTranslator
{
	// Single member types
	typedef int T::* IntMember_t;
	typedef float T::* FloatMember_t;
	typedef bool T::* BoolMember_t;
	typedef DynamicString T::* StringMember_t;
	// Array member types
	typedef Array<int> T::* IntArrayMember_t;
	typedef Array<float> T::* FloatArrayMember_t;
	typedef Array<bool> T::* BoolArrayMember_t;
	typedef Array<DynamicString> T::* StringArrayMember_t;

public:
	JsonDataTranslator()
	{

	}

	JsonDataTranslator& Add(HashedString name, IntMember_t member)
	{
		intMembers.Insert(name, member);
		return *this;
	}

	JsonDataTranslator& Add(HashedString name, FloatMember_t member)
	{
		floatMembers.Insert(name, member);
		return *this;
	}

	JsonDataTranslator& Add(HashedString name, BoolMember_t member)
	{
		boolMembers.Insert(name, member);
		return *this;
	}

	JsonDataTranslator& Add(HashedString name, StringMember_t member)
	{
		stringMembers.Insert(name, member);
		return *this;
	}

	JsonDataTranslator& Add(HashedString name, IntArrayMember_t member)
	{
		intArrayMembers.Insert(name, member);
		return *this;
	}

	JsonDataTranslator& Add(HashedString name, FloatArrayMember_t member)
	{
		floatArrayMembers.Insert(name, member);
		return *this;
	}

	JsonDataTranslator& Add(HashedString name, BoolArrayMember_t member)
	{
		boolArrayMembers.Insert(name, member);
		return *this;
	}

	JsonDataTranslator& Add(HashedString name, StringArrayMember_t member)
	{
		stringArrayMembers.Insert(name, member);
		return *this;
	}

	void TranslateJson(const rapidjson::Value& json, T *data) const
	{
		ASSERT(data != nullptr);

		for(auto it = json.MemberBegin(); it != json.MemberEnd(); ++it)
		{
			const HashedString name = HashedString::CreateFromDynamicString(it->name.GetString(), it->name.GetStringLength());

			switch(it->value.GetType())
			{
				case rapidjson::kNullType:
					break;

				case rapidjson::kFalseType:
				case rapidjson::kTrueType:
				{
					const BoolMember_t *memberPtr = boolMembers.Find(name);
					if(memberPtr != nullptr)
					{
						BoolMember_t member = *memberPtr;
						data->*member = it->value.GetBool();
					}
					break;
				}

				case rapidjson::kObjectType:
					break;

				case rapidjson::kArrayType:
					if(!it->value.Empty())
					{
						const rapidjson::Type arrayType = it->value[0].GetType();
						if(arrayType == rapidjson::kFalseType || arrayType == rapidjson::kTrueType)
						{
							const BoolArrayMember_t *memberPtr = boolArrayMembers.Find(name);
							if(memberPtr != nullptr)
							{
								BoolArrayMember_t member = *memberPtr;
								(data->*member).reserve(it->value.Size());
								for(rapidjson::Value::ConstValueIterator valueIt = it->value.Begin(); valueIt != it->value.End(); ++valueIt)
								{
									if(valueIt->IsBool())
									{
										(data->*member).push_back(valueIt->GetBool());
									}
								}
								break;
							}
						}
						else if(arrayType == rapidjson::kStringType)
						{
							const StringArrayMember_t *memberPtr = stringArrayMembers.Find(name);
							if(memberPtr != nullptr)
							{
								StringArrayMember_t member = *memberPtr;
								(data->*member).reserve(it->value.Size());
								for(rapidjson::Value::ConstValueIterator valueIt = it->value.Begin(); valueIt != it->value.End(); ++valueIt)
								{
									if(valueIt->IsString())
									{
										(data->*member).emplace_back(valueIt->GetString(), valueIt->GetStringLength());
									}
								}
								break;
							}
						}
						else if(arrayType == rapidjson::kNumberType)
						{
							if(it->value[0].IsInt())
							{
								const IntArrayMember_t *memberPtr = intArrayMembers.Find(name);
								if(memberPtr != nullptr)
								{
									IntArrayMember_t member = *memberPtr;
									(data->*member).reserve(it->value.Size());
									for(rapidjson::Value::ConstValueIterator valueIt = it->value.Begin(); valueIt != it->value.End(); ++valueIt)
									{
										if(valueIt->IsInt())
										{
											(data->*member).push_back(valueIt->GetInt());
										}
									}
								}
							}
							else
							{
								const FloatArrayMember_t *memberPtr = floatArrayMembers.Find(name);
								if(memberPtr != nullptr)
								{
									FloatArrayMember_t member = *memberPtr;
									(data->*member).reserve(it->value.Size());
									for(rapidjson::Value::ConstValueIterator valueIt = it->value.Begin(); valueIt != it->value.End(); ++valueIt)
									{
										if(valueIt->IsNumber())
										{
											(data->*member).push_back((float)valueIt->GetDouble());
										}
									}
									break;
								}
							}
						}
					}
					break;

				case rapidjson::kStringType:
				{
					const StringMember_t *memberPtr = stringMembers.Find(name);
					if(memberPtr != nullptr)
					{
						StringMember_t member = *memberPtr;
						data->*member = DynamicString(it->value.GetString(), it->value.GetStringLength());
					}
					break;
				}

				case rapidjson::kNumberType:
					if(it->value.IsInt())
					{
						const IntMember_t *memberPtr = intMembers.Find(name);
						if(memberPtr != nullptr)
						{
							IntMember_t member = *memberPtr;
							data->*member = it->value.GetInt();
						}
					}
					else
					{
						const FloatMember_t *memberPtr = floatMembers.Find(name);
						if(memberPtr != nullptr)
						{
							FloatMember_t member = *memberPtr;
							data->*member = (float)it->value.GetDouble();
						}
					}
					break;
			}
		}
	}

private:
	HashedStringMap<IntMember_t> intMembers;
	HashedStringMap<FloatMember_t> floatMembers;
	HashedStringMap<BoolMember_t> boolMembers;
	HashedStringMap<StringMember_t> stringMembers;

	HashedStringMap<IntArrayMember_t> intArrayMembers;
	HashedStringMap<FloatArrayMember_t> floatArrayMembers;
	HashedStringMap<BoolArrayMember_t> boolArrayMembers;
	HashedStringMap<StringArrayMember_t> stringArrayMembers;
};
