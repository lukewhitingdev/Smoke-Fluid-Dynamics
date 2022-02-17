#pragma once
#include "ComponentTypes.h"
#include <cstdint>
#include <Windows.h>

class Component
{
public:
	Component() : parent(nullptr), type(ComponentTypes::NOTSET), id(0) { id = reinterpret_cast<unsigned long long>(this); };

	ComponentTypes getType() { return type; };
	void setType(const ComponentTypes TYPE) { type = TYPE; };

	unsigned long long  getID() { return id; };
	void setParent(void* ent) { parent = ent; };
	void* getParent() { return parent; };

	void setRenderable(bool val) { renderable = val; };
	bool getRenderable() { return renderable; };

	void setUpdateable(bool val) { updateable = val; };
	bool getUpdatable() { return updateable; };

	virtual void Update(float deltaTime) { UNREFERENCED_PARAMETER(deltaTime); };
	virtual void Render() {};

private:
	void* parent;
	ComponentTypes type;
	unsigned long long id;
	bool renderable = true;
	bool updateable = true;
};