#pragma once
#include "Core/Entity System/Component.h"
#include <Windows.h>
class TestComponent : public Component
{
public:
	TestComponent() {};
	~TestComponent() {};

	void Render(float deltaTime) { UNREFERENCED_PARAMETER(deltaTime); };
	void Update(float deltaTime) { UNREFERENCED_PARAMETER(deltaTime); };

};

