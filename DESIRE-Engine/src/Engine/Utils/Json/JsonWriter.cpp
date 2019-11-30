#include "Engine/stdafx.h"
#include "Engine/Utils/Json/JsonWriter.h"

#include "Engine/Core/Math/Quat.h"
#include "Engine/Core/String/String.h"

JsonWriter::JsonWriter(rapidjson::StringBuffer& buffer)
	: writer(buffer)
{
	writer.StartObject();
}

JsonWriter::~JsonWriter()
{
	writer.EndObject();
}

void JsonWriter::AddInt32(const String& name, int32_t value)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.Int(value);
}

void JsonWriter::AddInt64(const String& name, int64_t value)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.Int64(value);
}

void JsonWriter::AddUint32(const String& name, uint32_t value)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.Uint(value);
}

void JsonWriter::AddUint64(const String& name, uint64_t value)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.Uint64(value);
}

void JsonWriter::AddFloat(const String& name, float value)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.Double(value);
}

void JsonWriter::AddBool(const String& name, bool value)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.Bool(value);
}

void JsonWriter::AddString(const String& name, const String& value)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.String(value.Str(), static_cast<rapidjson::SizeType>(value.Length()));
}

void JsonWriter::AddVector3(const String& name, const Vector3& value)
{
	float elements[3];
	value.StoreXYZ(elements);
	AddFloatArray(name, elements, 3);
}

void JsonWriter::AddQuat(const String& name, const Quat& value)
{
	float elements[4];
	value.StoreXYZW(elements);
	AddFloatArray(name, elements, 4);
}

void JsonWriter::AddInt32Array(const String& name, const int32_t* values, size_t numValues)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.StartArray();
	for(size_t i = 0; i < numValues; ++i)
	{
		writer.Int(values[i]);
	}
	writer.EndArray();
}

void JsonWriter::AddFloatArray(const String& name, const float* values, size_t numValues)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.StartArray();
	for(size_t i = 0; i < numValues; ++i)
	{
		writer.Double(values[i]);
	}
	writer.EndArray();
}

void JsonWriter::StartObject(const String& name)
{
	writer.Key(name.Str(), static_cast<rapidjson::SizeType>(name.Length()));
	writer.StartObject();
}

void JsonWriter::EndObject()
{
	writer.EndObject();
}
