#pragma once

#include "../3rdparty/rapidjson/include/rapidjson/document.h"

#include <stdint.h>

class Quat;
class String;
class Vector3;
class WritableString;

class JsonReader
{
public:
	JsonReader(rapidjson::Value::ConstObject jsonObject);

	// Basic types
	bool GetInt32(const String& name, int32_t& value) const;
	bool GetInt64(const String& name, int64_t& value) const;
	bool GetUint32(const String& name, uint32_t& value) const;
	bool GetUint64(const String& name, uint64_t& value) const;
	bool GetFloat(const String& name, float& value) const;
	bool GetBool(const String& name, bool& value) const;
	bool GetString(const String& name, WritableString& value) const;

	// Vectormath types
	bool GetVector3(const String& name, Vector3& value) const;
	bool GetQuat(const String& name, Quat& value) const;

	// Array types
	bool GetInt32Array(const String& name, int32_t* values, size_t numValues) const;
	bool GetFloatArray(const String& name, float* values, size_t numValues) const;

	// Object type
	JsonReader GetObject(const String& name) const;

private:
	rapidjson::Value::ConstObject json;

	friend class JsonObjectIterator;
};
