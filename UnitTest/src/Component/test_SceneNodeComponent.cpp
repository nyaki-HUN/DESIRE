#include "stdafx.h"
#include "Component/SceneNodeComponent.h"
#include "Scene/Transform.h"
#include "Scene/SceneGraphTraversal.h"

extern Transform preallocatedTransforms[10000];

TEST_CASE("SceneNodeComponent", "[Component]")
{
	SceneNodeComponent *rootNode = new SceneNodeComponent();

	SceneNodeComponent *child1 = new SceneNodeComponent();
	SceneNodeComponent *child2 = new SceneNodeComponent();
	SceneNodeComponent *child3 = new SceneNodeComponent();
	rootNode->AddChild(child1);
	rootNode->AddChild(child2);
	rootNode->AddChild(child3);
	REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &child2->GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child3->GetTransform());

	SceneNodeComponent *childA = new SceneNodeComponent();
	child1->AddChild(childA);
	REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child2->GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child3->GetTransform());

	SceneNodeComponent *childB = new SceneNodeComponent();
	SceneNodeComponent *childC = new SceneNodeComponent();
	child3->AddChild(childB);
	child3->AddChild(childC);
	REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child2->GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child3->GetTransform());
	REQUIRE((preallocatedTransforms + 5) == &childB->GetTransform());
	REQUIRE((preallocatedTransforms + 6) == &childC->GetTransform());

	SceneNodeComponent *childX = new SceneNodeComponent();
	SceneNodeComponent *childY = new SceneNodeComponent();
	childA->AddChild(childX);
	childB->AddChild(childY);
	REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &childX->GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child2->GetTransform());
	REQUIRE((preallocatedTransforms + 5) == &child3->GetTransform());
	REQUIRE((preallocatedTransforms + 6) == &childB->GetTransform());
	REQUIRE((preallocatedTransforms + 7) == &childY->GetTransform());
	REQUIRE((preallocatedTransforms + 8) == &childC->GetTransform());

	// Move 2 nodes by 1
	{
		// Move 'A' under '2' (2 nodes)
		child2->AddChild(childA);
		REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &child2->GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childA->GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &childX->GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3->GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB->GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY->GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC->GetTransform());

		// Move 'A' back
		child1->AddChild(childA);
		REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX->GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2->GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3->GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB->GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY->GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC->GetTransform());
	}

	// Move 2 nodes by 4
	{
		// Move 'A' under 'Y' (2 nodes)
		childY->AddChild(childA);
		REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &child2->GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &child3->GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &childB->GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &childY->GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childA->GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childX->GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC->GetTransform());

		// Move 'A' back
		child1->AddChild(childA);
		REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX->GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2->GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3->GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB->GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY->GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC->GetTransform());
	}

	// Move 4 nodes then remove 6 nodes
	{
		// Move '3' under 'A'
		childA->AddChild(child3);
		REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX->GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child3->GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &childB->GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childY->GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childC->GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &child2->GetTransform());

		// Remove 'A'
		childA->Remove();
		REQUIRE((preallocatedTransforms + 0) == &rootNode->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &child2->GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childA->GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &childX->GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3->GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB->GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY->GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC->GetTransform());
	}

	// "Update hierarchy"
	{
		rootNode->GetTransform().SetPosition(Vector3(1.0f, 1.0f, 1.0f));
		child1->GetTransform().SetPosition(Vector3(2.0f, 2.0f, 2.0f));

		REQUIRE((rootNode->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) != 0);
		REQUIRE((child1->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) != 0);
		
		rootNode->UpdateAllTransformsInHierarchy();

		CHECK((rootNode->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) == 0);
		CHECK((child1->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) == 0);

		Vector3 worldPos = rootNode->GetTransform().GetWorldPosition();
		CHECK_FLOATS(worldPos.GetX(), 1.0f);
		CHECK_FLOATS(worldPos.GetY(), 1.0f);
		CHECK_FLOATS(worldPos.GetZ(), 1.0f);

		worldPos = child1->GetTransform().GetWorldPosition();
		CHECK_FLOATS(worldPos.GetX(), 3.0f);
		CHECK_FLOATS(worldPos.GetY(), 3.0f);
		CHECK_FLOATS(worldPos.GetZ(), 3.0f);
	}

	struct TestPolicy
	{
		static bool Visit(SceneNodeComponent *node)
		{
			CHECK((node->GetTransform().GetFlags() & Transform::EFlags::SCALE_CHANGED) == 0);
			return true;
		}
	};

	const size_t traversedCount = SceneGraphTraversal<TestPolicy>::Traverse(rootNode);
	CHECK(traversedCount == 3);
}
