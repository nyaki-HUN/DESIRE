#include "stdafx.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"
#include "Scene/SceneGraphTraversal.h"

TEST_CASE("Object", "[Component]")
{
	Object *rootObj = new Object();
	const Transform *preallocatedTransforms = &rootObj->GetTransform();

	Object *child1 = new Object();
	Object *child2 = new Object();
	Object *child3 = new Object();
	rootObj->AddChild(child1);
	rootObj->AddChild(child2);
	rootObj->AddChild(child3);
	REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &child2->GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child3->GetTransform());

	Object *childA = new Object();
	child1->AddChild(childA);
	REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child2->GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child3->GetTransform());

	Object *childB = new Object();
	Object *childC = new Object();
	child3->AddChild(childB);
	child3->AddChild(childC);
	REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA->GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child2->GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child3->GetTransform());
	REQUIRE((preallocatedTransforms + 5) == &childB->GetTransform());
	REQUIRE((preallocatedTransforms + 6) == &childC->GetTransform());

	Object *childX = new Object();
	Object *childY = new Object();
	childA->AddChild(childX);
	childB->AddChild(childY);
	REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
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
		REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
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
		REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
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
		REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
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
		REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
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
		REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
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
		REQUIRE((preallocatedTransforms + 0) == &rootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1->GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &child2->GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childA->GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &childX->GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3->GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB->GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY->GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC->GetTransform());
		delete childA;
		childA = nullptr;
	}

	// "Update hierarchy"
	{
		rootObj->GetTransform().SetPosition(Vector3(1.0f, 1.0f, 1.0f));
		child1->GetTransform().SetPosition(Vector3(2.0f, 2.0f, 2.0f));

		REQUIRE((rootObj->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) != 0);
		REQUIRE((child1->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) != 0);
		
		rootObj->UpdateAllTransformsInHierarchy();

		CHECK((rootObj->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) == 0);
		CHECK((child1->GetTransform().GetFlags() & Transform::WORLD_MATRIX_DIRTY) == 0);

		Vector3 worldPos = rootObj->GetTransform().GetPosition();
		CHECK_FLOATS(worldPos.GetX(), 1.0f);
		CHECK_FLOATS(worldPos.GetY(), 1.0f);
		CHECK_FLOATS(worldPos.GetZ(), 1.0f);

		worldPos = child1->GetTransform().GetPosition();
		CHECK_FLOATS(worldPos.GetX(), 3.0f);
		CHECK_FLOATS(worldPos.GetY(), 3.0f);
		CHECK_FLOATS(worldPos.GetZ(), 3.0f);
	}

	const size_t traversedCount = SceneGraphTraversal::Traverse(rootObj, [](Object *node)
	{
		CHECK((node->GetTransform().GetFlags() & Transform::EFlags::SCALE_CHANGED) == 0);
		return true;
	});
	CHECK(traversedCount == 3);
}