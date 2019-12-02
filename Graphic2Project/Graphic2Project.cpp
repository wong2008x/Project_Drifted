// Graphic2Project.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Graphic2Project.h"
#include <d3d11.h>
#include <directxmath.h>
#pragma comment(lib,"d3d11.lib")
#include "PixelShader.csh"
#include "VertexShader.csh"

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

ID3D11Device* mDev;
IDXGISwapChain* mSwap;
ID3D11DeviceContext* mContext;
ID3D11RenderTargetView* mRTV;
D3D11_VIEWPORT mPort;
ID3D11Buffer* vBuff;
ID3D11InputLayout* vLayout;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
ID3D11VertexShader* vShader; //HLSL
ID3D11PixelShader* pShader;  //HLSL
ID3D11Buffer* cBuff; //Constant Buffer
//XMMATRIX                g_World;
//XMMATRIX                g_View;
//XMMATRIX                g_Projection;
float aspectRatio = 1.0f;

#define MAX_LOADSTRING 100



// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void CleanupDevice();
void Render();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHIC2PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHIC2PROJECT));

    MSG msg;

    // Main message loop:
    while (true)//GetMessage(&msg, nullptr, 0, 0))
    {
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		if (msg.message == WM_QUIT)
			break;

		//Render();

		float color[] = { 0, 0, 1, 1 };
		mContext->ClearRenderTargetView(mRTV, color);

		//Setup the pipeline

		//output merger
		ID3D11RenderTargetView* tempRTV[] = { mRTV };
		mContext->OMSetRenderTargets(1, tempRTV, nullptr);
		// rasterizer
		mContext->RSSetViewports(1, &mPort);
		// Input Assembler
		mContext->IASetInputLayout(vLayout);
		UINT strides[] = { sizeof(SimpleVertex) };
		UINT offsets[] = { 0 };
		ID3D11Buffer* tempVB[] = { vBuff };
		mContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		mContext->VSSetShader(vShader, 0, 0);
		mContext->PSSetShader(pShader, 0, 0);

		static float rotation = 0; rotation += 0.01f;
		XMMATRIX temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(0, 0, 2);
		XMMATRIX temp2 = XMMatrixRotationY(rotation);
		temp = XMMatrixMultiply(temp2, temp);
		XMStoreFloat4x4(&myMatricies.g_World,temp);

		//view
		temp = XMMatrixLookAtLH({ 2,2,-1 }, { 0,0,1 }, { 0,1,0 });
		XMStoreFloat4x4(&myMatricies.g_View, temp);

		//projection
		temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);


		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		HRESULT hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = myMatricies;
		mContext->Unmap(cBuff, 0);

		ID3D11Buffer* constants[] = { cBuff };
		mContext->VSSetConstantBuffers(0, 1, constants);


		mContext->Draw(numVerts, 0);

		mSwap->Present(1, 0);
    }
	CleanupDevice();

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHIC2PROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHIC2PROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   RECT mWinR;
   GetClientRect(hWnd, &mWinR);

   //my codes here   
   D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
   DXGI_SWAP_CHAIN_DESC swap;
   ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
   swap.BufferCount = 1;
   swap.OutputWindow = hWnd;
   swap.Windowed = true;
   swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
   swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   swap.BufferDesc.Width = mWinR.right - mWinR.left;
   swap.BufferDesc.Height = mWinR.bottom - mWinR.top;
   swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swap.SampleDesc.Count = 1;
   aspectRatio = swap.BufferDesc.Width / static_cast<float>(swap.BufferDesc.Height);
 
   HRESULT hr;
   // ----Debug Mode change later

   UINT createDeviceFlags = 0;
#ifdef _DEBUG
   createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

   hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, 
	   &dx11, 1, D3D11_SDK_VERSION, &swap, &mSwap, &mDev, 0, &mContext);

   ID3D11Resource* backBuffer;
   hr = mSwap->GetBuffer(0, __uuidof(backBuffer), (void**)&backBuffer);
   hr = mDev->CreateRenderTargetView(backBuffer, NULL, &mRTV);

   backBuffer->Release();

   //D3D_DRIVER_TYPE driverTypes[] =
   //{
	  // D3D_DRIVER_TYPE_HARDWARE,
	  // D3D_DRIVER_TYPE_WARP,
	  // D3D_DRIVER_TYPE_REFERENCE,
   //};
   //UINT numDriverTypes = ARRAYSIZE(driverTypes);

   //D3D_FEATURE_LEVEL featureLevels[] =
   //{
	  // D3D_FEATURE_LEVEL_11_1,
	  // D3D_FEATURE_LEVEL_11_0,
	  // D3D_FEATURE_LEVEL_10_1,
	  // D3D_FEATURE_LEVEL_10_0,
   //};
   //UINT numFeatureLevels = ARRAYSIZE(featureLevels);

   //for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
   //{
	  // g_driverType = driverTypes[driverTypeIndex];
	  // hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
		 //  D3D11_SDK_VERSION, &mDev, &g_featureLevel, &g_pImmediateContext);

	  // if (hr == E_INVALIDARG)
	  // {
		 //  // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
		 //  hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
			//   D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
	  // }

	  // if (SUCCEEDED(hr))
		 //  break;
   //}

   mPort.Height = swap.BufferDesc.Height;
   mPort.Width = swap.BufferDesc.Width;
   mPort.TopLeftX = mPort.TopLeftY = 0;
   mPort.MinDepth = 0;
   mPort.MaxDepth = 1;

   SimpleVertex tri[] = // NDC
   {
	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {0.25f, -0.25f, 0.25f, 1},  {1, 0, 1, 1} },
	   { {-0.25f, -0.25f, 0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {0.25f, -0.25f, 0.25f, 1},  {1, 0, 1, 1} },
	   { {0.25f, -0.25f, -0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {-0.25f, -0.25f, -0.25f, 1},  {1, 0, 1, 1} },
	   { {0.25f, -0.25f, -0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {-0.25f, -0.25f, -0.25f, 1},  {1, 0, 1, 1} },
	   { {-0.25f, -0.25f, 0.25f, 1},  {1, 1, 1, 1} },
   };
   numVerts = ARRAYSIZE(tri);
   //LOAD THE TRI
   D3D11_BUFFER_DESC bDesc;
   D3D11_SUBRESOURCE_DATA subData;
   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(SimpleVertex)*numVerts;
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DEFAULT;
   
   subData.pSysMem = tri;

   mDev->CreateBuffer(&bDesc, &subData, &vBuff);
  
   // compile the shaders
   hr=mDev->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
   hr=mDev->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);
   
   //Describe it to D3D11
   D3D11_INPUT_ELEMENT_DESC ieDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   hr= mDev->CreateInputLayout(ieDesc,2, VertexShader, sizeof(VertexShader), &vLayout);


   //Constant Buffer
   ZeroMemory(&bDesc, sizeof(bDesc));

   bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bDesc.ByteWidth = sizeof(WVP);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DYNAMIC;

   hr= mDev->CreateBuffer(&bDesc, nullptr, &cBuff);

   //// Initialize the world matrices
   //g_World = XMMatrixIdentity();

   //// Initialize the view matrix
   //XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
   //XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   //XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   //g_View = XMMatrixLookAtLH(Eye, At, Up);

   //// Initialize the projection matrix
   //g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.01f, 100.0f);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Render()
{
	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;
	}

	// Rotate cube around the origin
	//g_World = XMMatrixRotationY(t);

	//ConstantBuffer cb1;
	//cb1.mWorld = XMMatrixTranspose(g_World);
	//cb1.mView = XMMatrixTranspose(g_View);
	//cb1.mProjection = XMMatrixTranspose(g_Projection);

	//mContext-> UpdateSubresource(cBuff, 0, nullptr, &cb1, 0, 0);

	//Rendering here

	//float color[] = { 0, 0, 1, 1 };
	//mContext->ClearRenderTargetView(mRTV, color);

	////Setup the pipeline

	////output merger
	//ID3D11RenderTargetView* tempRTV[] = { mRTV };
	//mContext->OMSetRenderTargets(1, tempRTV, nullptr);
	//// rasterizer
	//mContext->RSSetViewports(1, &mPort);
	//// Input Assembler
	//mContext->IASetInputLayout(vLayout);
	//UINT strides[] = { sizeof(SimpleVertex) };
	//UINT offsets[] = { 0 };
	//ID3D11Buffer* tempVB[] = { vBuff };
	//mContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
	//mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);




	//D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	//HRESULT hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	//*((XMMATRIX)(gpuBuffer.pData)) =
			//Vertex Shader Stage
		mContext->VSSetShader(vShader, 0, 0);

		mContext->VSSetConstantBuffers(0, 1, &cBuff);

		//Pixel Shader
		mContext->PSSetShader(pShader, 0, 0);
		mContext->PSSetConstantBuffers(0, 1, &cBuff);

		mContext->Draw(numVerts, 0);
}
void CleanupDevice()
{
	mRTV->Release();
	mContext->Release();
	mSwap->Release();
	mDev->Release();
	vBuff->Release();
	if(cBuff)cBuff->Release();
	vLayout->Release();
	vShader->Release();
	pShader->Release();
}