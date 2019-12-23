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
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsNumber())
	{
		value = iter->value.GetInt();
		return true;
	}

	return false;
}

bool JsonReader::GetInt64(const String& name, int64_t& value) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsNumber())
	{
		value = iter->value.GetInt64();
		return true;
	}

	return false;
}

bool JsonReader::GetUint32(const String& name, uint32_t& value) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsNumber())
	{
		value = iter->value.GetUint();
		return true;
	}

	return false;
}

bool JsonReader::GetUint64(const String& name, uint64_t& value) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsNumber())
	{
		value = iter->value.GetUint64();
		return true;
	}

	return false;
}

bool JsonReader::GetFloat(const String& name, float& value) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsNumber())
	{
		value = iter->value.GetFloat();
		return true;
	}

	return false;
}

bool JsonReader::GetBool(const String& name, bool& value) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsBool())
	{
		value = iter->value.GetBool();
		return true;
	}

	return false;
}

bool JsonReader::GetString(const String& name, WritableString& value) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsString())
	{
		value.Set(iter->value.GetString(), iter->value.GetStringLength());
		return true;
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

bool JsonReader::GetInt32Array(const String& name, int32_t* values, size_t numValues) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsArray() && iter->value.Size() == numValues)
	{
		for(uint32_t i = 0; i < numValues; ++i)
		{
			values[i] = iter->value[i].GetInt();
		}

		return true;
	}

	return false;
}

bool JsonReader::GetFloatArray(const String& name, float* values, size_t numValues) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsArray() && iter->value.Size() == numValues)
	{
		for(uint32_t i = 0; i < numValues; ++i)
		{
			values[i] = iter->value[i].GetFloat();
		}

		return true;
	}

	return false;
}

JsonReader JsonReader::GetObject(const String& name) const
{
	auto iter = json.FindMember(rapidjson::GenericStringRef(name.Str(), static_cast<rapidjson::SizeType>(name.Length())));
	if(iter != json.MemberEnd() && iter->value.IsObject())
	{
		return JsonReader(iter->value.GetObject());
	}

	const rapidjson::Value emptyValue(rapidjson::kObjectType);
	return JsonReader(emptyValue.GetObject());
}
