#pragma once

#include "../3rdparty/rapidjson/include/rapidjson/prettywriter.h"

#include <stdint.h>

class Quat;
class String;
class Vector3;

class JsonWriter
{
public:
	JsonWriter(rapidjson::StringBuffer& buffer);
	~JsonWriter();

	// Basic types
	void AddInt32(const String& name, int32_t value);
	void AddInt64(const String& name, int64_t value);
	void AddUint32(const String& name, uint32_t value);
	void AddUint64(const String& name, uint64_t value);
	void AddFloat(const String& name, float value);
	void AddBool(const String& name, bool value);
	void AddString(const String& name, const String& value);

	// Vectormath types
	void AddVector3(const String& name, const Vector3& value);
	void AddQuat(const String& name, const Quat& value);

	// Array types
	void AddInt32Array(const String& name, const int32_t* values, size_t numValues);
	void AddFloatArray(const String& name, const float* values, size_t numValues);

	// Object type
	void StartObject(const String& name);
	void EndObject();

private:
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer;
};
