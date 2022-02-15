#pragma once
#include "Core/Entity System/Component.h"
#include <d3d11.h>
#include "Core/structures.h"
#include <Utility/Direct3D/Headers/D3D.h>
#include <DirectXMath.h>
#include "Dependencies/Textures/DDSTextureLoader.h"
class Material : public Component
{
public:
	Material();
	~Material();

	void Update(float deltaTime);
	void Render();

	ID3D11SamplerState** getTextureSamplerState() { return &sampler; }
	ID3D11Buffer* getMaterialConstantBuffer() { return materialConstantBuffer; }
	ID3D11ShaderResourceView** getTextureResourceView() { return &textureResourceView; }

private:
	ID3D11PixelShader* pixelShader = nullptr;

	MaterialConstantBuffer	materialCBProxy;
	ID3D11Buffer* materialConstantBuffer = nullptr;
	ID3D11ShaderResourceView* textureResourceView = nullptr;
	ID3D11SamplerState* sampler = nullptr;

	D3D* direct3D = nullptr;
};

