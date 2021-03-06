ScriptComponent@ otherScript;
float timeToCallOtherScript;

void Init()
{
	@otherScript = self;
	timeToCallOtherScript = 0.0f;
}

void Kill()
{
}

void Update()
{
	float deltaT = Timer.GetSecDelta();
	if(@otherScript != null)
	{
		timeToCallOtherScript -= deltaT;
		if(timeToCallOtherScript < 0.0f)
		{
			otherScript.Call("TestCallFromOtherScript", self);
			otherScript.Call("TestCallFromOtherScript2", 123, 10.0f);
			timeToCallOtherScript = 0.5f;
		}
	}

	Vector3 pos(1, 2, 3);
	Vector3 pos2 = pos;
	pos2.SetX(4);
	pos2 = pos2 * 10;
	pos = pos * pos2;
	pos += pos2;
	pos = -pos;

	Object@ object = self.GetObject();
	RenderComponent@ renderComp = object.GetComponent(kRenderComponent);
	if(@renderComp != null)
	{
		
	}

	object.GetTransform().SetPosition(pos);
	print("x = " + object.GetTransform().GetPosition().GetX());

	// Static function test
	Matrix4 mat = Matrix4::CreateTranslation(Vector3(100.0, 2.0, 0.0));
}

void TestCallFromOtherScript(ScriptComponent@ caller)
{
	print("TestCallFromOtherScript called by " + caller.GetObject().GetObjectName());
}

void TestCallFromOtherScript2(int arg1, float arg2)
{
	print("TestCallFromOtherScript2 called with: " + arg1 + ", " + arg2);
}

void Function1()
{
	print("Function1 called");
}

void Function2(float a1, uint32 a2)
{
	print("Function2 called with: " + a1 + ", " + a2);
}

void Function3(const string& in str)
{
	print("Function3 called with: " + str);
}
