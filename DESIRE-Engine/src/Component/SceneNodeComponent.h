#pragma once

#include "Component/IComponent.h"

#include <vector>

class Transform;
class AABB;

class SceneNodeComponent : public IComponent
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("SCEN");

public:
	enum EFlags
	{

	};

	SceneNodeComponent();
	~SceneNodeComponent();

	void Remove();

	void SetVisible(bool visible);

	// Add the child to this scene node (if the scene node already has a parent it is first removed)
	void AddChild(SceneNodeComponent *child);

	// Remove the child from this scene node
	void RemoveChild(SceneNodeComponent *child);

	Transform& GetTransform();
	const AABB& GetAABB() const;
	SceneNodeComponent* GetParent();
	const std::vector<SceneNodeComponent*>& GetChildren() const;

	void UpdateAllTransformsInHierarchy();

private:
	void SetNewParent(SceneNodeComponent *newParent);

	Transform *transform;
	AABB *aabb;
	size_t numTransformsInHierarchy;

	SceneNodeComponent *parent;
	std::vector<SceneNodeComponent*> children;

	int flags;
};
