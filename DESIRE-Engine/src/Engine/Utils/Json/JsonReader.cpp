#include "Engine/stdafx.h"
#include "Engine/Utils/Json/JsonReader.h"

#include "Engine/Core/Math/Quat.h"
#include "Engine/Core/String/WritableString.h"

JsonReader::JsonReader(rapidjson::Value::ConstObject jsonObject)
	: json(jsonObject)
{
}

bool JsonReader::GetInt32(const String& name, int32_t& value) const
{
	for(const auto& member : json)
	{
		if(member.value.IsInt() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			value = member.value.GetInt();
			return true;
		}
	}

	return false;
}

bool JsonReader::GetInt64(const String& name, int64_t& value) const
{
	for(const auto& member : json)
	{
		if(member.value.IsInt64() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			value = member.value.GetInt64();
			return true;
		}
	}

	return false;
}

bool JsonReader::GetUint32(const String& name, uint32_t& value) const
{
	for(const auto& member : json)
	{
		if(member.value.IsUint() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			value = member.value.GetUint();
			return true;
		}
	}

	return false;
}

bool JsonReader::GetUint64(const String& name, uint64_t& value) const
{
	for(const auto& member : json)
	{
		if(member.value.IsUint64() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			value = member.value.GetUint64();
			return true;
		}
	}

	return false;
}

bool JsonReader::GetFloat(const String& name, float& value) const
{
	for(const auto& member : json)
	{
		if(member.value.IsNumber() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			value = member.value.GetFloat();
			return true;
		}
	}

	return false;
}

bool JsonReader::GetBool(const String& name, bool& value) const
{
	for(const auto& member : json)
	{
		if(member.value.IsBool() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			value = member.value.GetBool();
			return true;
		}
	}

	return false;
}

bool JsonReader::GetString(const String& name, WritableString& value) const
{
	for(const auto& member : json)
	{
		if(member.value.IsString() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			value.Set(member.value.GetString(), member.value.GetStringLength());
			return true;
		}
	}

	return false;
}

bool JsonReader::GetVector3(const String& name, Vector3& value) const
{
	float elements[3];
	if(GetFloatArray(name, elements, 3))
	{
		value.LoadXYZ(elements);
		return true;
	}

	return false;
}

bool JsonReader::GetQuat(const String& name, Quat& value) const
{
	float elements[4];
	if(GetFloatArray(name, elements, 4))
	{
		value.LoadXYZW(elements);
		return true;
	}

	return false;
}

bool JsonReader::GetInt32Array(const String& name, int32_t* pValues, size_t numValues) const
{
	ASSERT(pValues != nullptr);

	for(const auto& member : json)
	{
		if(member.value.IsArray() && member.value.Size() == numValues && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			for(uint32_t i = 0; i < numValues; ++i)
			{
				pValues[i] = member.value[i].GetInt();
			}
			return true;
		}
	}

	return false;
}

bool JsonReader::GetFloatArray(const String& name, float* pValues, size_t numValues) const
{
	ASSERT(pValues != nullptr);

	for(const auto& member : json)
	{
		if(member.value.IsArray() && member.value.Size() == numValues && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			for(uint32_t i = 0; i < numValues; ++i)
			{
				pValues[i] = member.value[i].GetFloat();
			}
			return true;
		}
	}

	return false;
}

bool JsonReader::GetVector3Array(const String& name, Vector3* pValues, size_t numValues) const
{
	ASSERT(pValues != nullptr);

	for(const auto& member : json)
	{
		if(member.value.IsArray() && member.value.Size() == numValues * 3 && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			for(uint32_t i = 0; i < numValues; ++i)
			{
				pValues[i] = Vector3(member.value[i * 3 + 0].GetFloat(), member.value[i * 3 + 1].GetFloat(), member.value[i * 3 + 2].GetFloat());
			}
			return true;
		}
	}

	return false;
}

JsonReader JsonReader::GetObject(const String& name) const
{
	for(const auto& member : json)
	{
		if(member.value.IsObject() && String(member.name.GetString(), member.name.GetStringLength()).Equals(name))
		{
			return JsonReader(member.value.GetObject());
		}
	}

	const rapidjson::Value emptyValue(rapidjson::kObjectType);
	return JsonReader(emptyValue.GetObject());
}
