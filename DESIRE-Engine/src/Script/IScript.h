#pragma once

#include <functional>
#include <array>

class ScriptComponent;

class IScript
{
	friend class NativeScriptComponent;

protected:
	IScript() {}

public:
	struct Arg
	{
		enum class EType
		{
			UNDEFINED,
			INT,
			FLOAT,
			DOUBLE,
			BOOL,
			VOID_PTR,
			STRING
		};

		EType type;

		union
		{
			int inVal;
			float floatVal;
			double doubleVal;
			bool boolVal;
			void *voidPtrVal;
			const char *stringVal;
		};
	};

	virtual ~IScript() {}

	virtual void Init() = 0;
	virtual void Kill() = 0;
	virtual void Update() = 0;

	virtual std::function<void(std::array<IScript::Arg, 6>)> GetFunctionToCall(const char *functionName) = 0;

protected:
	ScriptComponent *self;
};
