#pragma once

class Object;
class String;

class AssimpLoader
{
public:
	static std::unique_ptr<Object> Load(const String& filename);
};
