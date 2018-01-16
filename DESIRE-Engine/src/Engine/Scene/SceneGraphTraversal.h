#pragma once

#include "Engine/Scene/Object.h"

#include <functional>

// --------------------------------------------------------------------------------------------------------------------
//	Recursive depth-first scene graph traversal which uses the 'visitFunc' for traversed nodes.
//	Return false from 'visitFunc' if you don't want to go deeper
// --------------------------------------------------------------------------------------------------------------------

class SceneGraphTraversal
{
public:
	static size_t Traverse(Object *rootObject, const std::function<bool(Object*)>& visitFunc)
	{
		size_t traversedNodeCount = 1;
		if(visitFunc(rootObject))
		{
			for(Object *obj : rootObject->GetChildren())
			{
				traversedNodeCount += Traverse(obj, visitFunc);
			}
		}
		return traversedNodeCount;
	}
};
