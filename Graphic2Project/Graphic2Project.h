#pragma once

#include "resource.h"
#include <d3d11.h>
#include <directxmath.h>
#pragma comment(lib,"d3d11.lib")
#include "PixelShader.csh"
#include "PixelMeshShader.csh"
#include "VertexShader.csh"
#include "VertexMeshShader.csh"
#include "VertexWaveShader.csh"
#include "SkyPixelShader.csh"
#include "SkyVertexShader.csh"
#include "Assets/StoneHenge.h"	
#include "Utillity/DDSTextureLoader.h"
#include <vector>
#include <string>
#include <fstream>
#include "Utillity/XTime.h"
#include <DirectXColors.h>

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
}myLighting;



//Forward declaration
void CleanupDevice();
void Render();
void postRender(D3D11_MAPPED_SUBRESOURCE gpuBuffer);
void Update();
void UpdateCamera();
bool loadObject(const char* path, std::vector <SimpleMesh>& outVertices, std::vector <unsigned int>& outIndicies, bool isRHCoord);
void WindowResize(UINT _width, UINT _height);
//Global Variable
unsigned int numVerts;



XTime mTimer;
double totalTime[2];
double delta_time = 0;
float cameraSpeed = 5.f;
bool multiviewPort = true;



HWND mWindow;
RECT mWinR;

ID3D11Device* mDev = nullptr;
IDXGISwapChain* mSwap = nullptr;
ID3D11DeviceContext* mContext = nullptr;
ID3D11RenderTargetView* mRTV = nullptr;
D3D11_VIEWPORT mPort;
D3D11_VIEWPORT mFirPort;
D3D11_VIEWPORT mSecPort;



//Lighting;
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


ID3D11Buffer* cBuff = nullptr; //Constant Buffer
ID3D11Buffer* timerBuff = nullptr; //Constant Buffer
ID3D11Buffer* cLightBuff = nullptr; //Constant Buffer
//primitive Triangle
ID3D11Buffer* vBuff = nullptr;
ID3D11InputLayout* vLayout = nullptr;
ID3D11VertexShader* vShader = nullptr; //HLSL
ID3D11PixelShader* pShader = nullptr;  //HLSL

XMFLOAT4 trianglePos;

//Skybox 
ID3D11Buffer* vSkyBuff = nullptr;
ID3D11Buffer* iSkyBuff = nullptr;
ID3D11VertexShader* vSkyShader = nullptr; //HLSL
ID3D11PixelShader* pSkyShader = nullptr;  //HLSL
ID3D11InputLayout* skyLayout = nullptr;
ID3D11ShaderResourceView* skyBoxTextureRV = nullptr;
ID3D11SamplerState* skyBoxSamplerState = nullptr;

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
XMFLOAT4 rockPos;
//Flag 
vector<SimpleMesh> flagVertex;
vector<unsigned int> flagIndices;
ID3D11Buffer* vFlagBuff = nullptr;
ID3D11Buffer* iFlagBuff = nullptr;
ID3D11VertexShader* vFlagShader = nullptr;//HLSL
ID3D11PixelShader* pFlagShader = nullptr;
ID3D11ShaderResourceView* flagTextureRV = nullptr;
ID3D11Texture2D* flagTexture = nullptr;
ID3D11InputLayout* vFlagLayout = nullptr;
ID3D11SamplerState* flagSamplerState = nullptr;
XMFLOAT4 flagPos;



ID3D11Buffer* vStoneBuff = nullptr;
ID3D11Buffer* iStoneBuff = nullptr;
ID3D11VertexShader* vStoneShader = nullptr;//HLSL
ID3D11PixelShader* pStoneShader = nullptr;
ID3D11ShaderResourceView* stoneTextureRV = nullptr;
ID3D11Texture2D* stoneTexture = nullptr;
ID3D11InputLayout* vStoneLayout = nullptr;
ID3D11SamplerState* stoneSamplerState= nullptr;
XMFLOAT4 stonePos;

ID3D11Buffer* sphereIndexBuffer = nullptr;
ID3D11Buffer* sphereVertBuffer = nullptr;


vector<XMFLOAT4> objPos;
ID3D11Texture2D* zBuffer = nullptr;
ID3D11DepthStencilView* zBufferView = nullptr;

float aspectRatio = 1.0f;
float FOV = 75.0f	; 
float nPlane = 0.01f;
float fPlane = 1000.0f;


//Camera

POINT lastPos{ -1,-1 };
POINT CurPos{ -1,-1 };

XMMATRIX camRotationMatrix;
XMMATRIX Rotationx;
XMMATRIX Rotationy;
XMMATRIX Rotationz;
XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;
XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;
float camYaw = 0.0f;
float camPitch = 0.0f;
bool cameraReset;
bool lookAT;
unsigned int curObj = 0;

//
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;