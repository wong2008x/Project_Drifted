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
#include "Assets/StoneHenge.h"	
#include "Utillity/DDSTextureLoader.h"
#include <vector>
#include <string>
#include <fstream>
#include "Utillity/XTime.h"

using namespace std;
using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT4 Pos;
	XMFLOAT4 Color;
};
struct SimpleMesh
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Norm;
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

//Forward declaration
void CleanupDevice();
void Render();
void UpdateCamera();
bool loadObject(const char* path, std::vector <SimpleMesh>& outVertices, std::vector <unsigned int>& outIndicies, bool isRHCoord);

//Global Variable
unsigned int numVerts;

XTime mTimer;
double delta_time = 0;
float cameraSpeed = 5.f;
ID3D11Device* mDev = nullptr;
IDXGISwapChain* mSwap = nullptr;
ID3D11DeviceContext* mContext = nullptr;
ID3D11RenderTargetView* mRTV = nullptr;
D3D11_VIEWPORT mPort;




D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;

ID3D11Buffer* vBuff = nullptr;
ID3D11InputLayout* vLayout = nullptr;
ID3D11VertexShader* vShader = nullptr; //HLSL
ID3D11PixelShader* pShader = nullptr;  //HLSL
ID3D11Buffer* cBuff = nullptr; //Constant Buffer

ID3D11Buffer* vBuff1 = nullptr;
ID3D11Buffer* iBuff1 = nullptr;
ID3D11VertexShader* vShader1 = nullptr; //HLSL
ID3D11PixelShader* pShader1 = nullptr;  //HLSL
ID3D11Buffer* cBuff1 = nullptr; //Constant Buffer


//Mesh Loader
vector<SimpleMesh> rockVertex;
vector<unsigned int> rockIndices;
ID3D11Buffer* vRockBuff = nullptr;
ID3D11Buffer* iRockBuff = nullptr;
ID3D11VertexShader* vRockShader = nullptr;//HLSL
ID3D11PixelShader* pRockShader = nullptr;
ID3D11ShaderResourceView* rockTextureRV = nullptr;
ID3D11Texture2D* rockTexture = nullptr;
ID3D11InputLayout* vRockLayout = nullptr;
ID3D11SamplerState* rockSamplerState = nullptr;

ID3D11Buffer* vStoneBuff = nullptr;
ID3D11Buffer* iStoneBuff = nullptr;
ID3D11VertexShader* vStoneShader = nullptr;//HLSL
ID3D11PixelShader* pStoneShader = nullptr;
ID3D11ShaderResourceView* stoneTextureRV = nullptr;
ID3D11Texture2D* stoneTexture = nullptr;
ID3D11InputLayout* vStoneLayout = nullptr;
ID3D11SamplerState* stoneSamplerState= nullptr;

ID3D11Texture2D* zBuffer = nullptr;
ID3D11DepthStencilView* zBufferView = nullptr;

float aspectRatio = 1.0f;
float FOV = 75.0f	; 
float nPlane = 0.01f;
float fPlane = 100.0f;
