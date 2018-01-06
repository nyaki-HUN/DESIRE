#pragma once

#include <memory>

// --------------------------------------------------------------------------------------------------------------------
//	GuardedCallbackFactory is a utility class template that creates callbacks which are guaranteed to be called only
//	when their factory is not destroyed.
//	It is intended to be used with lambda functions that doesn't have return value.
//	For convenience the returned Functor can also be used as an std::function
//	Example:
//		std::function<void(int)> callback = callbackFactory.CreateCallback([](int x){ ... });
// --------------------------------------------------------------------------------------------------------------------

class GuardedCallbackFactory
{
public:
	GuardedCallbackFactory()
		: guard(std::make_shared<uint8_t>((uint8_t)42))		// 42 - "Answer to the Ultimate Question of Life, the Universe, and Everything"
	{

	}

	// Inner class for storing the original callback function
	template<class F>
	class Functor
	{
		friend class GuardedCallbackFactory;

		Functor(F&& func, std::weak_ptr<uint8_t> guard)
			: func(std::move(func))
			, guard(guard)
		{

		}

	public:
		template<class... Args>
		void operator()(Args&&... args)
		{
			if(!guard.expired())
			{
				func(std::forward<Args>(args)...);
			}
		}

		template<class... Args>
		void operator()(Args&&... args) const
		{
			if(!guard.expired())
			{
				func(std::forward<Args>(args)...);
			}
		}

	private:
		F func;
		std::weak_ptr<uint8_t> guard;
	};

	// Creates a guarded callback
	template<class F>
	Functor<F> CreateCallback(F&& func)
	{
		return Functor<F>(std::move(func), guard);
	}

private:
	std::shared_ptr<uint8_t> guard;
};
