#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "Dependencies/Textures/DDSTextureLoader.h"
#include <iostream>
#include "Core/structures.h"
#include "Core/Entity System/Entity.h"
#include "Core/Components/Transform/Transform.h"
#include "Core/Components/Mesh/Mesh.h"
#include <Core/Components/Material/Material.h>
using namespace DirectX;

class GameObject : public Entity
{
public:
	GameObject();
	GameObject(const char* name);
	~GameObject();

	void cleanup();

	void								update(float t, ID3D11DeviceContext* pContext);
	void								draw(ID3D11DeviceContext* pContext);
	XMFLOAT4X4*							getWorld() { return transform->getWorld(); }
	Transform* getTransform() { return transform; }
	void removeMesh() { this->removeComponent<Mesh>(); mesh = nullptr; };
	void removeMaterial() { this->removeComponent<Material>(); material = nullptr; };
	void setPosition(XMFLOAT3 position);

	void setRenderable(bool val) { renderable = val; };
	bool getRenderable() { return renderable; };
	void setUpdateable(bool val) { updateable = val; };
	bool getUpdateable() { return updateable; };

private:
	
	bool renderable = true;
	bool updateable = true;

	Transform* transform;
	Mesh* mesh;
	Material* material;
};

