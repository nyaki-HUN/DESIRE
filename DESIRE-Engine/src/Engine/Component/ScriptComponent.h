#pragma once

#include "Engine/Component/Component.h"

class String;

class ScriptComponent : public Component
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("SCRI");

protected:
	ScriptComponent(Object& object);

public:
	enum class EBuiltinFuncType
	{
		Update,
		Init,
		Kill,
		Num
	};

	~ScriptComponent() override;

	// Call a built-in function (this is usually faster as the function pointer is cached)
	virtual void CallByType(EBuiltinFuncType funcType) = 0;

	// Call a script function with arbitrary arguments
	template<class... Args>
	void Call(const char *functionName, Args... args)
	{
		if(PrepareFunctionCall(functionName))
		{
			// Since initializer lists guarantee sequencing, this can be used to call AddFunctionCallArg on each element of 'args' in the same order
			std::initializer_list<bool> dummy = { (AddFunctionCallArg(args))... };
			// Do execute and cleanup
			ExecuteFunctionCall();
		}
	}

private:
	virtual bool PrepareFunctionCall(const char *functionName) = 0;
	virtual void ExecuteFunctionCall() = 0;

	// Add argument for function call
	virtual bool AddFunctionCallArg(int arg) = 0;
	virtual bool AddFunctionCallArg(float arg) = 0;
	virtual bool AddFunctionCallArg(double arg) = 0;
	virtual bool AddFunctionCallArg(bool arg) = 0;
	virtual bool AddFunctionCallArg(void *arg) = 0;
	virtual bool AddFunctionCallArg(const String& string) = 0;
};
