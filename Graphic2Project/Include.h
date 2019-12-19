
#include <string>
#include <thread>
#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")
#include <vector>
#include <fstream>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include	<random>
using namespace std;
using namespace DirectX;
#pragma once

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
	XMFLOAT4 Tangent;
};
struct SkyBox
{
	XMFLOAT3 Pos;
};

struct InstanceData
{
	XMFLOAT3 pos;
};


struct WVP
{
	XMFLOAT4X4                g_World;   //64
	XMFLOAT4X4                g_View;    //64
	XMFLOAT4X4                g_Projection; //64
};

struct CamConstant
{
	XMFLOAT4 camPos;
	BOOL hasNormal;
	BOOL hasMultiTex;
	BOOL hasShadowMap;
	FLOAT padding;
};
struct LightingConstant
{
	XMFLOAT4 dLightClr;
	XMFLOAT4 dLightDir;  //16
	XMFLOAT4 pLightClr;
	XMFLOAT4 pLightPos;//16
	XMFLOAT4 sLightClr;
	XMFLOAT4 sLightDir;//16
	XMFLOAT4 sLightPos;//16
	float innerAngle;//4
	float outerAngle;//4
	float pLightRadius; //4
	int lightingMode;//4
};