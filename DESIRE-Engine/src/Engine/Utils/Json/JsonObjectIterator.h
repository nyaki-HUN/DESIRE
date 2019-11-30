#pragma once

#include "Engine/Utils/Json/JsonReader.h"

#include "Engine/Core/String/String.h"

#include "../3rdparty/rapidjson/include/rapidjson/document.h"

class JsonObjectIterator
{
public:
	JsonObjectIterator(const JsonReader& reader);

	bool IsValid() const;
	String GetName() const;
	JsonReader GetJsonReader() const;

	void Next();

private:
	rapidjson::Value::ConstMemberIterator iter;
	rapidjson::Value::ConstMemberIterator end;
};