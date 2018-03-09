#pragma once

#include <memory>

class Render;

class Modules
{
public:
	static std::unique_ptr<Render> Render;
};
