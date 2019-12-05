#pragma once

#include "resource.h"
#include <d3d11.h>
#include <directxmath.h>
#pragma comment(lib,"d3d11.lib")
#include "PixelShader.csh"
#include "PixelMeshShader.csh"
#include "VertexShader.csh"
#include "VertexMeshShader.csh"
#include "Assets/Rock.h"	
#include "Utillity/DDSTextureLoader.h"
#include <vector>

using namespace std;
using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT4 Pos;
	XMFLOAT4 Color;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 vLightDir[2];
	XMFLOAT4 vLightColor[2];
	XMFLOAT4 vOutputColor;
};

struct WVP
{
	XMFLOAT4X4                g_World;
	XMFLOAT4X4                g_View;
	XMFLOAT4X4                g_Projection;
}myMatricies;

unsigned int numVerts;

ID3D11Device* mDev = nullptr;
IDXGISwapChain* mSwap = nullptr;
ID3D11DeviceContext* mContext = nullptr;
ID3D11RenderTargetView* mRTV = nullptr;
D3D11_VIEWPORT mPort;

ID3D11Buffer* vBuff = nullptr;

ID3D11InputLayout* vLayout = nullptr;
ID3D11InputLayout* vMeshLayout = nullptr;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;

ID3D11VertexShader* vShader = nullptr; //HLSL
ID3D11PixelShader* pShader = nullptr;  //HLSL

ID3D11Buffer* cBuff = nullptr; //Constant Buffer

//Mesh Loader
ID3D11Buffer* vBuffMesh = nullptr;
ID3D11Buffer* iBuffMesh = nullptr;
ID3D11VertexShader* vMeshShader = nullptr;//HLSL
ID3D11PixelShader* pMeshShader = nullptr;
ID3D11ShaderResourceView* vTextureRV = nullptr;
ID3D11Texture2D* enviromentTexture = nullptr;
ID3D11SamplerState* mSamplerLinear = nullptr;

ID3D11Texture2D* zBuffer = nullptr;
ID3D11DepthStencilView* zBufferView = nullptr;



float aspectRatio = 1.0f;
float cameraSpeed=0.01f;