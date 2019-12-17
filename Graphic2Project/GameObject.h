#pragma once
#include "Include.h"
class GameObject
{
public:
	vector<SimpleMesh> GO_vertex;
	ID3D11Buffer* pGO_Vbuff = nullptr;
	ID3D11InputLayout* pGO_inputLayout = nullptr;
	ID3D11PixelShader* pGO_PS = nullptr;
	ID3D11VertexShader* pGO_VS = nullptr;
	ID3D11ShaderResourceView* pGO_SRV_Texture = nullptr;
	ID3D11ShaderResourceView* pGO_SRV_secTexture = nullptr;
	ID3D11ShaderResourceView* pGO_SRV_NormTex = nullptr;
	unsigned int stride = sizeof(SimpleMesh);
	XMFLOAT4 objPos;
	ID3D11Device* mDevice = nullptr;
	GameObject();
	void CreateGameObject(ID3D11Device* _device, const char* _filepath, const BYTE* _VS, unsigned int _VSize);
	~GameObject();
};

