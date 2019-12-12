#pragma once

#include "resource.h"
#include <d3d11.h>
#include <directxmath.h>
#pragma comment(lib,"d3d11.lib")
#include "PixelShader.csh"
#include "PixelMeshShader.csh"
#include "VertexShader.csh"
#include "VertexMeshShader.csh"
#include "SkyPixelShader.csh"
#include "SkyVertexShader.csh"
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
	XMFLOAT3 Tex;
	XMFLOAT3 Norm;
};
struct SkyBox
{
	XMFLOAT3 Pos;
};


struct WVP
{
	XMFLOAT4X4                g_World;   //64
	XMFLOAT4X4                g_View;    //64
	XMFLOAT4X4                g_Projection; //64

}myMatricies;

struct LightingConstant
{
	XMFLOAT4 dLightDir;  //16
	XMFLOAT4 pLightPos;//16
	XMFLOAT4 sLightDir;//16
	XMFLOAT4 sLightPos;//16
	FLOAT innerAngle;//4
	FLOAT outerAngle;//4
	FLOAT pLightRadius; //4
	BOOL lightingMode;//4
	DOUBLE worldTime;//8//  1 pack
	XMFLOAT2 padding;
}myLighting;

//Forward declaration
void CleanupDevice();
void Render();
void Update();
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

//Lighting
const XMVECTOR dLightD = { -0.557f,-0.557f,0.557f,1 };
const XMVECTOR pLightPosD = { -10.0f,1.0f,1.0f,1 };
const XMVECTOR pLightRadiusD = { 15.0f};

const XMVECTOR sLightDirD = { -0.707f,-0.707f,0.0f,1 };
const XMVECTOR sLightPosD = { 0.0f,7.0f,0.0f,1 };
const XMVECTOR innerAngleD = { 0.9f };
const XMVECTOR outerAngleD = { 0.8f };

//May want to reset;
XMVECTOR dLight = { -0.557f,-0.557f,0.557f,1 };
XMVECTOR pLightPos = { -10.0f,1.0f,1.0f,1 };
XMVECTOR pLightRadius = { 15.0f };

XMVECTOR sLightDir = { -0.707f,-0.707f,0.0f,1 };
XMVECTOR sLightPos = { 0.0f,7.0f,0.0f,1 };
XMVECTOR innerAngle = { 0.9f };
XMVECTOR outerAngle = {0.8f};
bool flag = true;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;

//primitive Triangle
ID3D11Buffer* vBuff = nullptr;
ID3D11InputLayout* vLayout = nullptr;
ID3D11VertexShader* vShader = nullptr; //HLSL
ID3D11PixelShader* pShader = nullptr;  //HLSL
ID3D11Buffer* cBuff = nullptr; //Constant Buffer

//Skybox 
ID3D11Buffer* vSkyBuff = nullptr;
ID3D11Buffer* iSkyBuff = nullptr;
ID3D11VertexShader* vSkyShader = nullptr; //HLSL
ID3D11PixelShader* pSkyShader = nullptr;  //HLSL
ID3D11InputLayout* skyLayout = nullptr;
ID3D11Buffer* cLightBuff = nullptr; //Constant Buffer


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
