otherScript = nil;
timeToCallOtherScript = 0.0;

function Init()
	otherScript = self;
	timeToCallOtherScript = 0.0;
end

function Kill()

end

function Update()
	local deltaT = Timer:GetSecDelta();
	if(otherScript ~= nil) then
		timeToCallOtherScript = timeToCallOtherScript - deltaT;
		if(timeToCallOtherScript < 0.0) then
			otherScript:Call("TestCallFromOtherScript", self);
			otherScript:Call("TestCallFromOtherScript2", 123, 10.0);
			timeToCallOtherScript = 0.5;
		end
	end

	local pos = Vector3.new(1, 2, 3);
	local pos2 = pos;
	pos2.x = 4;
	pos = pos2 * 4;
	pos = pos + pos2;
	pos = -pos;

	local renderComp = self.object:GetRenderComponent();
	if(renderComp ~= nil) then
		
	end

	self.object.transform.position = pos;
	print("x = ", self.object.transform.position.x);

	-- Static function test
	local mat = Matrix4.CreateTranslation(Vector3.new(100.0, 2.0, 0.0));
end

function TestCallFromOtherScript(caller)
	print("TestCallFromOtherScript called by ", caller.object:GetObjectName());
end

function TestCallFromOtherScript2(arg1, arg2)
	print("TestCallFromOtherScript2 called with: ", arg1, ", ", arg2);
end

function Function1()
	print("Function1 called");
end

function Function2(a1, a2)
	print("Function2 called with: ", a1, ", ", a2);
end

function Function3(str)
	print("Function3 called with: ", str);
end
