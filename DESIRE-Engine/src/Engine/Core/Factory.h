#pragma once

template<class T, class... Args>
class Factory
{
public:
	typedef std::unique_ptr<T>(*Func_t)(Args&&...);

	template<class C>
	static std::unique_ptr<T> Create(Args&&... args)
	{
		return std::make_unique<C>(std::forward<Args>(args)...);
	}
};
