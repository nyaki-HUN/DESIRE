#pragma once

#include "Engine/Core/Object.h"

// --------------------------------------------------------------------------------------------------------------------
//	Recursive depth-first scene graph traversal which uses the 'visitFunc' for traversed nodes.
//	Return false from 'visitFunc' if you don't want to go deeper
// --------------------------------------------------------------------------------------------------------------------

class SceneGraphTraversal
{
public:
	static size_t Traverse(Object& rootObject, const std::function<bool(Object&)>& visitFunc)
	{
		size_t traversedNodeCount = 1;
		if(visitFunc(rootObject))
		{
			for(Object* pObject : rootObject.GetChildren())
			{
				traversedNodeCount += Traverse(*pObject, visitFunc);
			}
		}
		return traversedNodeCount;
	}
};
