#pragma once

#include "Component/SceneNodeComponent.h"

// --------------------------------------------------------------------------------------------------------------------
//	Recursive depth-first scene graph traversal which uses the VisitPolicy for traversed nodes.
//	You can create custom visit policy by simply creating a struct with the method: bool Visit(SceneNodeComponent *node);
// --------------------------------------------------------------------------------------------------------------------

template<typename VisitPolicy>
struct SSceneGraphTraversal : public VisitPolicy
{
	static size_t Traverse(SceneNodeComponent *rootNode)
	{
		size_t traversedNodeCount = 1;
		if(VisitPolicy::Visit(rootNode))
		{
			for(SceneNodeComponent *node : rootNode->GetChildren())
			{
				traversedNodeCount += Traverse(node);
			}
		}
		return traversedNodeCount;
	}
};
