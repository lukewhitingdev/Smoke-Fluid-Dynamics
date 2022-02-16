#include "GameObject.h"

using namespace std;
using namespace DirectX;

#define NUM_VERTICES 36

GameObject::GameObject()
{
	transform = this->addComponent<Transform>();
	mesh = this->addComponent<Mesh>();
	material = this->addComponent<Material>();
}

GameObject::GameObject(const char* name)
{
	transform = this->addComponent<Transform>();
	mesh = this->addComponent<Mesh>();
	material = this->addComponent<Material>();
	this->setName(name);
}


GameObject::~GameObject()
{
	cleanup();
}

void GameObject::cleanup()
{
}

void GameObject::setPosition(XMFLOAT3 position)
{
	transform->setPosition(position);
}

void GameObject::update(float t, ID3D11DeviceContext* pContext)
{
	UNREFERENCED_PARAMETER(pContext);
	for(auto it = components.begin(); it != components.end(); it++)
	{
		std::vector<Component*>* componentVector = &it->second;
		for(int i = 0; i < componentVector->size(); i++)
		{
			componentVector->at(i)->Update(t);
		}
	}
}

void GameObject::draw(ID3D11DeviceContext* pContext)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		std::vector<Component*>* componentVector = &it->second;
		for (int i = 0; i < componentVector->size(); i++)
		{
			componentVector->at(i)->Render();
		}
	}
	pContext->DrawIndexed(NUM_VERTICES, 0, 0);
}
