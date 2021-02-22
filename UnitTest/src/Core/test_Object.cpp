#include "stdafx.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Math/Transform.h"

#include "Engine/Scene/SceneGraphTraversal.h"

TEST_CASE("Object", "[Core]")
{
	Object* pRootObj = new Object();
	const Transform* preallocatedTransforms = &pRootObj->GetTransform();

/*	   R
	  /|\
	 / | \
	1  2  3
	|    / \
	A   B   C
	|   |
	X   Y
*/
	Object& child1 = pRootObj->CreateChildObject("1");
	Object& child2 = pRootObj->CreateChildObject("2");
	Object& child3 = pRootObj->CreateChildObject("3");
	REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &child2.GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child3.GetTransform());

	Object& childA = child1.CreateChildObject("A");
	REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child2.GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child3.GetTransform());

	Object& childB = child3.CreateChildObject("B");
	Object& childC = child3.CreateChildObject("C");
	REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &child2.GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child3.GetTransform());
	REQUIRE((preallocatedTransforms + 5) == &childB.GetTransform());
	REQUIRE((preallocatedTransforms + 6) == &childC.GetTransform());

	Object& childX = childA.CreateChildObject("X");
	Object& childY = childB.CreateChildObject("Y");
	REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
	REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
	REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
	REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
	REQUIRE((preallocatedTransforms + 4) == &child2.GetTransform());
	REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
	REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
	REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
	REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());

	// Move 2 nodes by 1
	{
		// Move 'A' under '2' (2 nodes)
		childA.SetParent(&child2);
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());

		// Move 'A' back
		childA.SetParent(&child1);
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());
	}

	// Move 2 nodes by 4
	{
		// Move 'A' under 'Y' (2 nodes)
		childA.SetParent(&childY);
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());

		// Move 'A' back
		childA.SetParent(&child1);
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());
	}

	{
		// Move 'B' under root (2 nodes)
		childB.SetParent(pRootObj);
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childC.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childY.GetTransform());

		// Move 'C' under root (1 node)
		childC.SetParent(pRootObj);
		// The transform order goes back to the original one (dipite that it is a different hierarchy)
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());

		// Move 'B' back
		childB.SetParent(&child3);
		// The transform order should stay the same
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());

		// Move 'C' back
		childC.SetParent(&child3);
		// The transform order should stay the same
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());
	}

	// Move 4 nodes then remove 6 nodes
	{
		// Move '3' under 'A'
		child3.SetParent(&childA);
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childC.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &child2.GetTransform());

		// Remove 'A'
		childA.SetParent(nullptr);
		REQUIRE((preallocatedTransforms + 0) == &pRootObj->GetTransform());
		REQUIRE((preallocatedTransforms + 1) == &child1.GetTransform());
		REQUIRE((preallocatedTransforms + 2) == &child2.GetTransform());
		REQUIRE((preallocatedTransforms + 3) == &childA.GetTransform());
		REQUIRE((preallocatedTransforms + 4) == &childX.GetTransform());
		REQUIRE((preallocatedTransforms + 5) == &child3.GetTransform());
		REQUIRE((preallocatedTransforms + 6) == &childB.GetTransform());
		REQUIRE((preallocatedTransforms + 7) == &childY.GetTransform());
		REQUIRE((preallocatedTransforms + 8) == &childC.GetTransform());
		delete &childA;
	}

/*	   R
	  /|
	 / |
	1  2
*/

	// Absolut position update in hierarchy
	{
		child2.GetTransform().SetLocalPosition(Vector3(2.0f, 2.0f, 2.0f));
		Vector3 worldPos = child2.GetTransform().GetPosition();
		CHECK(worldPos.GetX() == Approx(2.0f));
		CHECK(worldPos.GetY() == Approx(2.0f));
		CHECK(worldPos.GetZ() == Approx(2.0f));

		pRootObj->GetTransform().SetLocalPosition(Vector3(1.0f, 1.0f, 1.0f));
		worldPos = pRootObj->GetTransform().GetPosition();
		CHECK(worldPos.GetX() == Approx(1.0f));
		CHECK(worldPos.GetY() == Approx(1.0f));
		CHECK(worldPos.GetZ() == Approx(1.0f));

		worldPos = child1.GetTransform().GetPosition();
		CHECK(worldPos.GetX() == Approx(1.0f));
		CHECK(worldPos.GetY() == Approx(1.0f));
		CHECK(worldPos.GetZ() == Approx(1.0f));

		worldPos = child2.GetTransform().GetPosition();
		CHECK(worldPos.GetX() == Approx(3.0f));
		CHECK(worldPos.GetY() == Approx(3.0f));
		CHECK(worldPos.GetZ() == Approx(3.0f));
	}

	{
		Object& child1_A = child1.CreateChildObject("2A");
		Object& child1_B = child1_A.CreateChildObject("2B");

		Vector3 worldPos = child1_A.GetTransform().GetPosition();
		CHECK(worldPos.GetX() == Approx(1.0f));
		CHECK(worldPos.GetY() == Approx(1.0f));
		CHECK(worldPos.GetZ() == Approx(1.0f));

		worldPos = child1_B.GetTransform().GetPosition();
		CHECK(worldPos.GetX() == Approx(1.0f));
		CHECK(worldPos.GetY() == Approx(1.0f));
		CHECK(worldPos.GetZ() == Approx(1.0f));

		child1_B.GetTransform().SetLocalPosition(Vector3(10.0f, 10.0f, 10.0f));
		worldPos = child1_B.GetTransform().GetPosition();
		CHECK(worldPos.GetX() == Approx(11.0f));
		CHECK(worldPos.GetY() == Approx(11.0f));
		CHECK(worldPos.GetZ() == Approx(11.0f));

		delete &child1_A;
	}

	const size_t traversedCount = SceneGraphTraversal::Traverse(*pRootObj,
		[](Object& object)
		{
			object.GetTransform().SetScale(Vector3(5.0f, 5.0f, 5.0f));
			return true;
		}
	);
	CHECK(traversedCount == 3);
}
