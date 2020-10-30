#pragma once

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
		: m_spGuard(std::make_shared<uint8_t>(static_cast<uint8_t>(42)))		// 42 - "Answer to the Ultimate Question of Life, the Universe, and Everything"
	{
	}

	// Inner class for storing the original callback function
	template<class F>
	class Functor
	{
		Functor(F&& function, std::weak_ptr<uint8_t> guard)
			: m_function(std::move(function))
			, m_guard(guard)
		{
		}

	public:
		template<class... Args>
		void operator()(Args&&... args)
		{
			if(!m_guard.expired())
			{
				m_function(std::forward<Args>(args)...);
			}
		}

		template<class... Args>
		void operator()(Args&&... args) const
		{
			if(!m_guard.expired())
			{
				m_function(std::forward<Args>(args)...);
			}
		}

	private:
		F m_function;
		std::weak_ptr<uint8_t> m_guard;

		friend class GuardedCallbackFactory;
	};

	// Creates a guarded callback
	template<class F>
	Functor<F> CreateCallback(F&& function)
	{
		return Functor<F>(std::move(function), m_spGuard);
	}

private:
	std::shared_ptr<uint8_t> m_spGuard;
};
