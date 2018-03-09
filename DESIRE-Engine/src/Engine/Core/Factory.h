#pragma once

#include <memory>

template<class T>
class Factory
{
public:
	typedef std::unique_ptr<T>(*Func_t)();

	template<class C>
	static std::unique_ptr<T> Create()
	{
		return std::make_unique<C>();
	}
};
