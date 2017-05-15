#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This macro implements singleton behaviour using lazy initialization. In C++11 standard, it is thread safe. 
//	§6.7[stmt.dcl] p4: "If control enters the declaration concurrently while the variable is being initialized,
//	the concurrent execution shall wait for completion of the initialization."
//	Copy constructor, move constructor, assignment operator and move assignment operator are forbidden
// --------------------------------------------------------------------------------------------------------------------

#define DESIRE_DECLARE_SINGLETON(Class)					\
public:													\
	static Class* Get()									\
	{													\
		static Class instance;							\
		return &instance;								\
	}													\
private:												\
	Class();											\
	~Class();											\
	Class(const Class&) = delete;						\
	Class(Class&&) = delete;							\
	Class& operator=(const Class&) = delete;			\
	Class& operator=(Class&&) = delete;


#define DESIRE_DECLARE_SINGLETON_INTERFACE(Class)		\
public:													\
	static Class* Get();								\
protected:												\
	Class() {}											\
	virtual ~Class() {}									\
	Class(const Class&) = delete;						\
	Class(Class&&) = delete;							\
	Class& operator=(const Class&) = delete;			\
	Class& operator=(Class&&) = delete;

#define DESIRE_DEFINE_SINGLETON_INSTANCE(Class, Impl)	\
	Class* Class::Get()									\
	{													\
		static Impl instance;							\
		return &instance;								\
	}	

#define DESIRE_DEFINE_EMPTY_SINGLETON_INSTANCE(Class)	\
	Class* Class::Get()									\
	{													\
		return nullptr;									\
	}	
