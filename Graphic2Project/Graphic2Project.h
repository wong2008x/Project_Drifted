#pragma once

#include "resource.h"
#include "Shader.h"

#include "Camera.h"
#include "Assets/StoneHenge.h"	
#include "Utillity/DDSTextureLoader.h"
#include "Utillity/XTime.h"
#include "Include.h"
#include "GameObject.h"



//Forward declaration

void CleanupDevice();
void Render();
void ThemeOne(WVP &myMatrix);
void ThemeTwo(WVP &myMatrix);
void Update();
bool loadObject(const char* path, std::vector <SimpleMesh>& outVertices);
void WindowResize(UINT _width, UINT _height);
void LoadGameObject();



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

XMVECTOR sLightDir = { -1.0f,-1.0f,0.0f,1 };
XMVECTOR sLightPos = { 0.0f,7.0f,0.0f,1 };
XMVECTOR innerAngle = { 0.9f };
XMVECTOR outerAngle = {0.85f};
bool flag = true;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;


ID3D11Buffer* cBuff = nullptr; //Constant Buffer
ID3D11Buffer* timerBuff = nullptr; //Constant Buffer
ID3D11Buffer* cLightBuff = nullptr; //Constant Buffer
ID3D11Buffer* camBuff = nullptr;

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
ID3D11Buffer* vRockBuff = nullptr;
ID3D11VertexShader* vRockShader = nullptr;//HLSL
ID3D11PixelShader* pRockShader = nullptr;
ID3D11ShaderResourceView* rockTextureRV=nullptr;
ID3D11ShaderResourceView* rockTextureRV1 = nullptr;
ID3D11Texture2D* rockTexture = nullptr;
ID3D11InputLayout* vRockLayout = nullptr;
ID3D11SamplerState* rockSamplerState = nullptr;
XMFLOAT4 rockPos;

//Flag 
vector<SimpleMesh> flagVertex;
ID3D11Buffer* vFlagBuff = nullptr;
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

ID3D11Buffer* spIndexBuffer = nullptr;
ID3D11Buffer* spVertBuffer = nullptr;
ID3D11InputLayout* spLayout = nullptr;
ID3D11VertexShader* spVShader = nullptr; //HLSL
ID3D11PixelShader* spPShader = nullptr;  //HLSL

//Theme1
GameObject island;
GameObject ground;
GameObject palmTree;


//Theme 2
GameObject spaceSkybox;
GameObject spaceShip;
GameObject sun;
GameObject earth;
GameObject mars;
GameObject satellite;
GameObject moon;

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
Camera* curCamera;
Camera firstCam;
Camera secCam;

XMFLOAT4 camPos;


bool lookAT;
bool SceneOne = true;
unsigned int curObj = 0;

//
XMMATRIX Rotationx;
XMMATRIX Rotationy;
XMMATRIX Rotationz;
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;

LightingConstant myLighting;
LightingConstant mySecLighting;

CamConstant myfirCamCons;
CamConstant mysecCamCons;

WVP myMatricies;
WVP mySecWorld;