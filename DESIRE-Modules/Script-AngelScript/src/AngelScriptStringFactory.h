#pragma once

#include "Engine/Core/platform.h"
#include "Engine/Core/String.h"

#include <map>

DESIRE_DISABLE_WARNINGS
#include "angelscript.h"
DESIRE_ENABLE_WARNINGS

class AngelScriptStringFactory : public asIStringFactory
{
public:
	AngelScriptStringFactory();
	~AngelScriptStringFactory() override;

	const void* GetStringConstant(const char *data, asUINT length) override;
	int ReleaseStringConstant(const void *str) override;

	int GetRawStringData(const void *str, char *data, asUINT *length) const override;

private:
	std::map<String, int> stringCache;
};