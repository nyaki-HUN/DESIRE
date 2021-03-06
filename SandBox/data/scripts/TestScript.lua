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
	local pos2 = Vector3.new(pos);
	pos2:SetX(4);
	pos2 = pos2 * 10;
	pos = pos * pos2;
	pos = pos + pos2;
	pos = -pos;

	local object = self:GetObject();
	local renderComp = object:GetRenderComponent();
	if(renderComp ~= nil) then
		
	end

	object:GetTransform():SetPosition(pos);
	print("x = ", object:GetTransform():GetPosition():GetX());

	-- Static function test
	local mat = Matrix4.CreateTranslation(Vector3.new(100.0, 2.0, 0.0));
end

function TestCallFromOtherScript(caller)
	print("TestCallFromOtherScript called by ", caller:GetObject():GetObjectName());
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
