#pragma once

// Compile-time four-character-code generation from string
constexpr int MakeFourCC(const char(&c)[5])
{
	return (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
}

#define DECLARE_COMPONENT_FURCC_TYPE_ID(STR_ID)						\
public:																\
	static const int TYPE_ID = MakeFourCC(STR_ID);					\
	inline int GetTypeID() const override final { return TYPE_ID; }

class Object;

class Component
{
protected:
	Component(Object& object);

public:
	virtual ~Component();

	void Destroy() const;

	virtual int GetTypeID() const = 0;

	inline Object& GetObject() const
	{
		return object;
	}

protected:
	Object& object;
};
