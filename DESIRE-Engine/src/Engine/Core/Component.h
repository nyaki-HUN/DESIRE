#pragma once

class EditorComponent;
class Object;

class Component
{
protected:
	Component(Object& object);

public:
	virtual ~Component();

	void Destroy();

	virtual void SetEnabled(bool value);
	bool IsEnabled() const;

	Object& GetObject() const;

	virtual void CloneTo(Object& otherObject) const = 0;
	virtual int32_t GetTypeId() const = 0;

	virtual EditorComponent* AsEditorComponent() { return nullptr; }

protected:
	Object& m_object;

private:
	bool m_enabled = true;
};

// Compile-time four-character-code generation from string
constexpr int32_t MakeFourCC(const char(&c)[5])
{
	return (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
}

#define DECLARE_COMPONENT_FOURCC_TYPE_ID(STR_ID)					\
public:																\
	static constexpr int32_t kTypeId = MakeFourCC(STR_ID);			\
	int32_t GetTypeId() const override final { return kTypeId; }
