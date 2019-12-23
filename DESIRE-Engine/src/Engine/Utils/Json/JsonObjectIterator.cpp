#include "Engine/stdafx.h"
#include "Engine/Utils/Json/JsonObjectIterator.h"

JsonObjectIterator::JsonObjectIterator(const JsonReader& reader)
	: iter(reader.json.MemberBegin())
	, end(reader.json.MemberEnd())
{
	if(iter != end && !iter->value.IsObject())
	{
		Next();
	}
}

bool JsonObjectIterator::IsValid() const
{
	return (iter != end);
}

String JsonObjectIterator::GetName() const
{
	return String(iter->name.GetString(), iter->name.GetStringLength());
}

JsonReader JsonObjectIterator::GetJsonReader() const
{
	return JsonReader(iter->value.GetObject());
}

void JsonObjectIterator::Next()
{
	if(IsValid())
	{
		do
		{
			++iter;
		} while(iter != end && !iter->value.IsObject());
	}
}
